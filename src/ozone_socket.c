#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define OZONE_SOCKET_POLLING_TIMEOUT_SECONDS 10
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192

#if defined(__APPLE__) || defined(__FreeBSD__)
#define OZONE_SOCKET_USE_KQUEUE 1
#endif

#if OZONE_SOCKET_USE_KQUEUE
#include <sys/event.h>
typedef struct kevent OzoneSocketConnection;
#define ozoneSocketConnectionFile(_event_) ((int)((_event_)->ident))

#else
#include <sys/epoll.h>
typedef struct epoll_event OzoneSocketConnection;
#define ozoneSocketConnectionFile(_event_) ((_event_)->data.fd)

#endif

OZONE_VECTOR_DECLARE_API(OzoneSocketConnection)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketConnection)

OZONE_VECTOR_IMPLEMENT_API(OzoneSocketHandlerRef)

typedef struct OzoneSocketWorkerStruct {
  pthread_t thread;
  const OzoneSocketHandlerRefVector* handler_pipeline;
  OzoneSocketConnection* connection;
  void* event_context;
} OzoneSocketWorker;

OZONE_VECTOR_DECLARE_API(OzoneSocketWorker)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketWorker)

int ozone_socket_shutdown = 0;
void ozoneSocketSignalAction(int signum) {
  if (signum == SIGINT) {
    ozoneLogWarn("Received SIGINT, will shutdown gracefully");
    ozone_socket_shutdown = 1;
  }
}

void* ozoneSocketHandleWorker(OzoneSocketWorker* worker) {
  struct timespec sleep_on_idle = (struct timespec) { .tv_nsec = 100000 };
  OzoneAllocator* event_allocator = NULL;
  while (!ozone_socket_shutdown) {
    int connection_fd = worker->connection ? ozoneSocketConnectionFile(worker->connection) : 0;
    if (!connection_fd) {
      nanosleep(&sleep_on_idle, NULL);
      continue;
    }

    int close_connection = 0;

    if (!event_allocator)
      event_allocator = ozoneAllocatorCreate(1024 + OZONE_SOCKET_REQUEST_CHUNK_SIZE);

    ozoneLogDebug("Worker picked up connection %d", connection_fd);
    ozoneAllocatorClear(event_allocator);
    OzoneSocketEvent event = { .allocator = event_allocator };

    int recv_code = 0;
    do {
      OzoneString* string = ozoneAllocatorReserveOne(event.allocator, OzoneString);
      string->vector = ozoneVectorAllocate(event.allocator, OzoneByte, OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1);

      recv_code = recv(connection_fd, ozoneStringBuffer(string), OZONE_SOCKET_REQUEST_CHUNK_SIZE, 0);
      if (recv_code == 0) {
        ozoneLogDebug("Client closed connection %d", connection_fd);
        close_connection = 1;
        break;
      }

      if (recv_code < 0) {
        if (errno == EWOULDBLOCK)
          continue;

        if (errno == ECONNRESET || errno == ECONNREFUSED || errno == ENOTCONN) {
          ozoneLogDebug("Client closed connection %d", connection_fd);
          close_connection = 1;
          break;
        }

        if (errno == EBADF) {
          ozoneLogWarn(
              "Tried to read from what may be a previously closed connection, errno %d for file %d",
              errno,
              connection_fd);
          close_connection = 1;
          break;
        }

        ozoneLogError("Could not read part of TCP connection request, errno %d for file %d", errno, connection_fd);
        close_connection = 1;
        break;
      }

      string->vector.length = (size_t)recv_code;
      ozoneVectorPushOzoneString(event.allocator, &event.raw_socket_request, string);
    } while (recv_code >= OZONE_SOCKET_REQUEST_CHUNK_SIZE);

    if (!close_connection && ozoneVectorLength(&event.raw_socket_request)) {
      ozoneLogDebug("Handling event on connection %d", connection_fd);
      OzoneSocketHandlerRef* handler;
      ozoneVectorForEach(handler, worker->handler_pipeline) (*handler)(&event, worker->event_context);

      OzoneString* chunk;
      ozoneVectorForEach(chunk, &event.raw_socket_response)
          send(connection_fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk), MSG_NOSIGNAL);
    }

    ozoneLogDebug("Closing connection %d", connection_fd);
    close(connection_fd);
    *worker->connection = (OzoneSocketConnection) { 0 };
    worker->connection = NULL;
  }

  ozoneAllocatorDelete(event_allocator);

  return NULL;
}

int ozoneSocketServeTCP(OzoneSocketConfig* config, void* context) {
  size_t connection_pool_capacity = SOMAXCONN - 1;
  if (config->max_connections && config->max_connections < connection_pool_capacity) {
    connection_pool_capacity = config->max_connections;
  }

  size_t worker_pool_capacity = 32;
  if (config->max_workers && config->max_workers < worker_pool_capacity) {
    worker_pool_capacity = config->max_workers;
  }

  struct sigaction signal_actions = { .sa_handler = &ozoneSocketSignalAction };
  sigaction(SIGINT, &signal_actions, NULL);

  int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    ozoneLogError("Failed to get AF_INET6 SOCK_STREAM socket file descriptor, returning EACCES");
    return EACCES;
  }

  // todo: review socket options, consider SO_LINGER
  const int socket_option_one = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_one, sizeof(int));
  if (ioctl(socket_fd, FIONBIO, &socket_option_one) < 0)
    ozoneLogWarn("Connection ioctl FIONBIO failed, socket may run in blocking mode");

  OzoneSocketConnection listening_socket;

#ifdef OZONE_SOCKET_USE_KQUEUE
  struct timespec kqueue_timeout = (struct timespec) { .tv_sec = OZONE_SOCKET_POLLING_TIMEOUT_SECONDS };

  int polling_fd = kqueue();
  if (polling_fd == -1) {
    ozoneLogError("Failed to get kqueue handle, returning EACCES");
    close(socket_fd);
    return EACCES;
  }

  EV_SET(&listening_socket, socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

  if (kevent(polling_fd, &listening_socket, 1, NULL, 0, NULL) == -1) {
    ozoneLogError("Failed to add socket polling event to kqueue handle, returning EACCES");
    close(socket_fd);
    close(polling_fd);
    return EACCES;
  }
#else
  int polling_fd = epoll_create1(0);
  if (polling_fd == -1) {
    ozoneLogError("Failed to get epoll file descriptor, returning EACCES");
    close(socket_fd);
    return EACCES;
  }

  listening_socket.events = EPOLLIN;
  listening_socket.data.fd = socket_fd;

  if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, socket_fd, &listening_socket)) {
    ozoneLogError("Failed to add socket polling event to epoll, returning EACCES");
    close(socket_fd);
    close(polling_fd);
    return EACCES;
  }
#endif

  struct sockaddr_in6 host_addr = { 0 };
  socklen_t host_addr_len = sizeof(host_addr);
  host_addr.sin6_family = AF_INET6;
  host_addr.sin6_port = htons(config->port);
  if (bind(socket_fd, (struct sockaddr*)&host_addr, host_addr_len) != 0) {
    ozoneLogError("Bind failed, returning ECONNABORTED");
    return ECONNABORTED;
  }

  if (listen(socket_fd, SOMAXCONN) != 0) {
    ozoneLogError("Listen on port %d failed", config->port);
    return ECONNABORTED;
  }

  ozoneLogDebug(
      "Listening for TCP connections on port %d with a %ld member handler_pipeline",
      config->port,
      ozoneVectorLength(&config->handler_pipeline));

  OzoneAllocator* socket_allocator = ozoneAllocatorCreate(1024);
  OzoneSocketConnectionVector connection_pool
      = ozoneVectorAllocate(socket_allocator, OzoneSocketConnection, connection_pool_capacity);
  connection_pool.length = connection_pool.capacity;
  ozoneLogDebug("Connection pool will handle up to %ld connections", ozoneVectorLength(&connection_pool));

#ifdef OZONE_SOCKET_USE_KQUEUE
  OzoneSocketConnectionVector events
      = ozoneVectorAllocate(socket_allocator, OzoneSocketConnection, connection_pool_capacity);
  events.length = connection_pool.capacity;
#else
  OzoneSocketConnectionVector events
      = ozoneVectorAllocate(socket_allocator, OzoneSocketConnection, connection_pool_capacity + 1);
  ozoneVectorPushOzoneSocketConnection(socket_allocator, &events, &listening_socket);
  events.length = connection_pool.capacity;
#endif

  OzoneSocketWorkerVector worker_pool = ozoneVectorAllocate(socket_allocator, OzoneSocketWorker, worker_pool_capacity);
  worker_pool.length = worker_pool.capacity;
  OzoneSocketWorker* worker;
  ozoneVectorForEach(worker, &worker_pool) {
    worker->event_context = context;
    worker->handler_pipeline = &config->handler_pipeline;
    pthread_create(&worker->thread, NULL, (void* (*)(void*))ozoneSocketHandleWorker, worker);
  }
  ozoneLogDebug("Created %ld workers", ozoneVectorLength(&worker_pool));

  struct timespec sleep_on_idle = (struct timespec) { .tv_nsec = 10000 };
  while (!ozone_socket_shutdown) {

#ifdef OZONE_SOCKET_USE_KQUEUE
    events.length = (size_t)kevent(polling_fd, NULL, 0, ozoneVectorBegin(&events), events.capacity, &kqueue_timeout);
#else
    epoll_wait(
        polling_fd, ozoneVectorBegin(&events), ozoneVectorLength(&events), OZONE_SOCKET_POLLING_TIMEOUT_SECONDS * 1000);
#endif

    OzoneSocketConnection* event;
    ozoneVectorForEach(event, &events) {
      if (ozone_socket_shutdown)
        break;

      int connection_fd = ozoneSocketConnectionFile(event);
      if (!connection_fd)
        break;

      while (connection_fd == socket_fd) {
        if (ozone_socket_shutdown)
          break;

        int accepted_socket_fd = accept(
            ozoneSocketConnectionFile(&listening_socket), (struct sockaddr*)&host_addr, (socklen_t*)&host_addr_len);
        if (accepted_socket_fd == -1)
          break;

        ozoneLogDebug("Accepted connection %d", accepted_socket_fd);
        for (;;) {
          OzoneSocketConnection* accepted_connection = NULL;
          OzoneSocketConnection* connection;
          ozoneVectorForEach(connection, &connection_pool) {
            if (!ozoneSocketConnectionFile(connection)) {
              accepted_connection = connection;
              break;
            }
          }

          if (!accepted_connection) {
            ozoneLogDebug("Waiting to handle connection, all connections in pool are occupied");
            nanosleep(&sleep_on_idle, NULL);
            continue;
          }

#ifdef OZONE_SOCKET_USE_KQUEUE
          EV_SET(accepted_connection, accepted_socket_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
          if (kevent(polling_fd, accepted_connection, 1, NULL, 0, NULL)) {
            ozoneLogError("Could not poll connection %d", accepted_socket_fd);
            close(accepted_socket_fd);
            *accepted_connection = (OzoneSocketConnection) { 0 };
          }
#else
          accepted_connection->data.fd = accepted_socket_fd;
          accepted_connection->events = EPOLLIN;

          if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, accepted_socket_fd, accepted_connection)) {
            ozoneLogError("Could not poll connection %d", accepted_socket_fd);
            close(accepted_socket_fd);
            *accepted_connection = (OzoneSocketConnection) { 0 };
          }

          break;
#endif
        }

        break;
      }

      while (connection_fd != socket_fd) {
        OzoneSocketConnection* accepted_connection = NULL;
        OzoneSocketConnection* connection;
        ozoneVectorForEach(connection, &connection_pool) {
          if (ozoneSocketConnectionFile(connection) == connection_fd) {
            accepted_connection = connection;
            break;
          }
        }

        if (!accepted_connection)
          break;

        int locked = 0;
        OzoneSocketWorker* accepted_worker = NULL;
        ozoneVectorForEach(worker, &worker_pool) {
          if (accepted_connection == worker->connection) {
            accepted_worker = NULL;
            locked = 1;
            break;
          }

          if (!accepted_worker && !worker->connection) {
            accepted_worker = worker;
          }
        }

        if (locked)
          break;

        if (!accepted_worker) {
          ozoneLogDebug("Waiting to handle connection, all workers in pool are occupied");
          nanosleep(&sleep_on_idle, NULL);
          continue;
        }

        accepted_worker->connection = accepted_connection;
        break;
      }
    }
  }

  ozoneVectorForEach(worker, &worker_pool) {
    if (worker->thread)
      pthread_join(worker->thread, NULL);
  }

  OzoneSocketConnection* connection;
  ozoneVectorForEach(connection, &connection_pool) {
    if (ozoneSocketConnectionFile(connection))
      close(ozoneSocketConnectionFile(connection));
  }

  ozoneAllocatorDelete(socket_allocator);
  close(polling_fd);
  close(socket_fd);

  return 0;
}
