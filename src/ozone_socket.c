#include "ozone_socket.h"

#include "ozone_generator.h"
#include "ozone_log.h"
#include "ozone_time.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define OZONE_SOCKET_EVENT_POLLING_CAPACITY 64
#define OZONE_SOCKET_IDLE_NS 100000
#define OZONE_SOCKET_INIT_ALLOCATOR 32768
#define OZONE_SOCKET_POLLING_TIMEOUT_MS 5000
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192

OZONE_VECTOR_IMPLEMENT_API(OzoneSocketHandlerRef)

#if defined(__APPLE__) || defined(__FreeBSD__)
#define OZONE_SOCKET_USE_KQUEUE 1
#endif

#if OZONE_SOCKET_USE_KQUEUE
#include <sys/event.h>
typedef struct kevent OzonePollingEvent;
#else
#include <sys/epoll.h>
typedef struct epoll_event OzonePollingEvent;
#endif

OZONE_VECTOR_DECLARE_API(OzonePollingEvent)
OZONE_VECTOR_IMPLEMENT_API(OzonePollingEvent)

typedef int SocketFd;
OZONE_VECTOR_DECLARE_API(SocketFd)
OZONE_VECTOR_IMPLEMENT_API(SocketFd)

typedef struct OzoneSocketConnectionDataStruct {
  OzoneSocketConfig* config;
  int socket_fd;
  OzoneSocketEvent socket_event;
  int recv_code;
  int recv_errno;
  char recv_buffer[OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1];
  size_t send_chunk_index;
  int send_code;
  int send_errno;
} OzoneSocketConnectionData;

OZONE_GENERATOR_DECLARE_API(OzoneSocketConnectionData)

typedef OzoneSocketConnectionDataGenerator OzoneSocketConnection;

OZONE_VECTOR_DECLARE_API(OzoneSocketConnection)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketConnection)

typedef struct OzoneSocketWorkerStruct {
  int id;
  pthread_t thread;
  OzoneSocketConfig* config;
  OzoneAllocator* sockets_allocator;
  // Architecturally, sparse arrays indexed by socket make much more sense here
  SocketFdVector* sockets_processing;
  SocketFdVector* sockets_waiting;
  pthread_mutex_t* sockets_write_lock;
} OzoneSocketWorker;

OZONE_VECTOR_DECLARE_API(OzoneSocketWorker)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketWorker)

static int ozone_socket_shutdown = 0;

void ozoneSocketShutdownSignalAction(int signum) {
  (void)signum;
  ozoneLogWarn("Received shutdown signal");
  ozone_socket_shutdown = 1;
}

int ozoneSocketSetupListener(
    int port,
    int* listening_socket_fd,
    int* polling_fd,
    struct sockaddr_in6* host_addr,
    socklen_t* host_addr_len,
    struct timeval* request_timeout_ms) {
  *listening_socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (*listening_socket_fd == -1) {
    ozoneLogError("Failed to get AF_INET6 SOCK_STREAM socket file descriptor, returning EACCES");
    return EACCES;
  }

  // todo: review socket options, consider SO_LINGER
  const int socket_option_one = 1;
  setsockopt(*listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_one, sizeof(int));
  setsockopt(*listening_socket_fd, SOL_SOCKET, SO_RCVTIMEO, request_timeout_ms, sizeof(struct timeval));

  if (ioctl(*listening_socket_fd, FIONBIO, &socket_option_one) < 0)
    ozoneLogWarn("Connection ioctl FIONBIO failed, socket may run in blocking mode");

  OzonePollingEvent listening_socket_event;

#ifdef OZONE_SOCKET_USE_KQUEUE
  *polling_fd = kqueue();
  if (*polling_fd == -1) {
    ozoneLogError("Failed to get kqueue handle, returning EACCES");
    close(*listening_socket_fd);
    return EACCES;
  }

  EV_SET(&listening_socket_event, *listening_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

  if (kevent(*polling_fd, &listening_socket_event, 1, NULL, 0, NULL) == -1) {
    ozoneLogError("Failed to add socket polling event to kqueue handle, returning EACCES");
    close(*listening_socket_fd);
    close(*polling_fd);
    return EACCES;
  }
#else
  *polling_fd = epoll_create1(0);
  if (*polling_fd == -1) {
    ozoneLogError("Failed to get epoll file descriptor, returning EACCES");
    close(*listening_socket_fd);
    return EACCES;
  }

  listening_socket_event.events = EPOLLIN | EPOLLET;
  listening_socket_event.data.fd = *listening_socket_fd;

  if (epoll_ctl(*polling_fd, EPOLL_CTL_ADD, *listening_socket_fd, &listening_socket_event)) {
    ozoneLogError("Failed to add socket polling event to epoll, returning EACCES");
    close(*listening_socket_fd);
    close(*polling_fd);
    return EACCES;
  }
#endif

  if (bind(*listening_socket_fd, (struct sockaddr*)host_addr, *host_addr_len) != 0) {
    ozoneLogError("Bind failed, returning ECONNABORTED");
    close(*listening_socket_fd);
    close(*polling_fd);
    return ECONNABORTED;
  }

  if (listen(*listening_socket_fd, SOMAXCONN) != 0) {
    ozoneLogError("Listen on port %d failed", port);
    close(*listening_socket_fd);
    close(*polling_fd);
    return ECONNABORTED;
  }

  return 0;
}

void ozoneSocketConnectionHandler(OzoneSocketConnection* connection) {
  OzoneSocketConnectionData* conn = &connection->context;
  ozoneGeneratorBegin(connection);

  gettimeofday(&conn->socket_event.time_begin, NULL);

  do {
    conn->recv_code = recv(conn->socket_fd, conn->recv_buffer, OZONE_SOCKET_REQUEST_CHUNK_SIZE, MSG_DONTWAIT);
    conn->recv_errno = conn->recv_code == -1 ? errno : 0;
    gettimeofday(&conn->socket_event.time_end, NULL);
    if (conn->recv_code == 0)
      break;

    if (conn->recv_code == -1) {
      if (conn->recv_errno != EAGAIN && conn->recv_errno != EWOULDBLOCK) {
        ozoneLogError("Received errno %d when recv on socket %d", errno, conn->socket_fd);
        close(conn->socket_fd);
        ozoneGeneratorReject(connection);
      }

      if (ozoneTimeDifferenceMilliseconds(&conn->socket_event.time_begin, &conn->socket_event.time_end)
          > connection->context.config->request_timeout_ms) {
        close(conn->socket_fd);
        ozoneGeneratorReject(connection);
      }

      ozoneGeneratorYield(connection);
      continue;
    }

    OzoneStringVectorPush(
        conn->socket_event.allocator,
        &conn->socket_event.raw_socket_request,
        ozoneStringFromBuffer(conn->socket_event.allocator, conn->recv_buffer, (size_t)conn->recv_code));
  } while (conn->recv_code == -1 || conn->recv_code == OZONE_SOCKET_REQUEST_CHUNK_SIZE);

  if (!conn->socket_event.raw_socket_request.length) {
    close(conn->socket_fd);
    ozoneGeneratorReject(connection);
  }

  OzoneSocketHandlerRef* handler;
  ozoneVectorForEach(handler, &conn->config->handler_pipeline) {
    (*handler)(&conn->socket_event);
    gettimeofday(&conn->socket_event.time_end, NULL);
    if (ozoneTimeDifferenceMilliseconds(&conn->socket_event.time_begin, &conn->socket_event.time_end)
        > connection->context.config->request_timeout_ms) {
      close(conn->socket_fd);
      ozoneGeneratorReject(connection);
    }
  }

  while (conn->send_chunk_index < conn->socket_event.raw_socket_response.length) {
    conn->send_code = send(
        conn->socket_fd,
        ozoneStringBuffer(&ozoneVectorAt(&conn->socket_event.raw_socket_response, conn->send_chunk_index)),
        ozoneStringLength(&ozoneVectorAt(&conn->socket_event.raw_socket_response, conn->send_chunk_index)),
        MSG_DONTWAIT);
    conn->send_errno = conn->send_code == -1 ? errno : 0;
    gettimeofday(&conn->socket_event.time_end, NULL);

    if (conn->send_code == -1 && conn->send_errno != EAGAIN && conn->send_errno != EWOULDBLOCK) {
      ozoneLogError("Received errno %d when send on socket %d", conn->send_errno, conn->socket_fd);
      close(conn->socket_fd);
      ozoneGeneratorReject(connection);
    }

    if (conn->send_code
        == (int)ozoneStringLength(&ozoneVectorAt(&conn->socket_event.raw_socket_response, conn->send_chunk_index))) {
      conn->send_chunk_index++;
      continue;
    }

    if (ozoneTimeDifferenceMilliseconds(&conn->socket_event.time_begin, &conn->socket_event.time_end)
        > connection->context.config->request_timeout_ms) {
      close(conn->socket_fd);
      ozoneGeneratorReject(connection);
    }

    ozoneGeneratorYield(connection);
  }

  ozoneGeneratorResolve(connection);
}

void* ozoneSocketHandleWorker(OzoneSocketWorker* worker) {
  ozoneLogInfo("Worker %d is up", worker->id);
  OzoneAllocator* allocator = ozoneAllocatorCreate(OZONE_SOCKET_INIT_ALLOCATOR);
  OzoneSocketConnectionVector connections = (OzoneSocketConnectionVector) { 0 };

  while (!ozone_socket_shutdown) {
    OzoneSocketConnection* connection_it;
    ozoneVectorForEach(connection_it, &connections) {
      ozoneSocketConnectionHandler(connection_it);
      if (ozoneGeneratorPending(connection_it))
        continue;

      int socket_fd_to_clear = connection_it->context.socket_fd;
      pthread_mutex_lock(worker->sockets_write_lock);
      int* socket_fd_it;
      ozoneVectorForEach(socket_fd_it, worker->sockets_processing) {
        if (*socket_fd_it == socket_fd_to_clear)
          *socket_fd_it = 0;
        break;
      }

      while (worker->sockets_processing->length && !*ozoneVectorLast(worker->sockets_processing))
        SocketFdVectorPop(worker->sockets_processing, NULL);

      pthread_mutex_unlock(worker->sockets_write_lock);
    }

    while (connections.length && !ozoneGeneratorPending(ozoneVectorLast(&connections))) {
      OzoneSocketConnectionVectorPop(&connections, NULL);
    }

    pthread_mutex_lock(worker->sockets_write_lock);
    int socket_fd = 0;
    SocketFdVectorShift(worker->sockets_waiting, &socket_fd);

    if (socket_fd) {
      int sockets_processing_reused = 0;
      int* socket_fd_it;
      ozoneVectorForEach(socket_fd_it, worker->sockets_processing) {
        if (!*socket_fd_it) {
          *socket_fd_it = socket_fd;
          sockets_processing_reused = 1;
          break;
        }
      }

      if (!sockets_processing_reused)
        SocketFdVectorPush(worker->sockets_allocator, worker->sockets_processing, &socket_fd);
    }

    pthread_mutex_unlock(worker->sockets_write_lock);

    if (!socket_fd) {
      if (!connections.length) {
        connections = (OzoneSocketConnectionVector) { 0 };
        ozoneAllocatorClear(allocator);
        nanosleep(&(struct timespec) { .tv_nsec = OZONE_SOCKET_IDLE_NS }, NULL);
      }

      continue;
    }

    OzoneSocketConnection* connection = ozoneAllocatorReserveOne(allocator, OzoneSocketConnection);
    *connection = ozoneGenerator(
        OzoneSocketConnectionData,
        ((OzoneSocketConnectionData) {
            .config = worker->config,
            .socket_fd = socket_fd,
            .socket_event = ((OzoneSocketEvent) {
                .allocator = allocator,
                .context = worker->config->handler_context,
            }),
        }));

    int connection_reused = 0;
    ozoneVectorForEach(connection_it, &connections) {
      if (!ozoneGeneratorPending(connection_it)) {
        *connection_it = *connection;
        connection_reused = 1;
        break;
      }
    }

    if (!connection_reused)
      OzoneSocketConnectionVectorPush(allocator, &connections, connection);
  }

  ozoneAllocatorDelete(allocator);
  ozoneLogInfo("Worker %d is down", worker->id);
  return NULL;
}

int ozoneSocketServeTCP(OzoneSocketConfig* config) {
  int listening_socket_fd, polling_fd;
  struct sockaddr_in6 host_addr = (struct sockaddr_in6) {
    .sin6_family = AF_INET6,
    .sin6_port = htons(config->port),
  };
  socklen_t host_addr_len = sizeof(host_addr);
  struct timeval request_timeout_ms = (struct timeval) { .tv_usec = config->request_timeout_ms * 1000 };
  int err = ozoneSocketSetupListener(
      config->port, &listening_socket_fd, &polling_fd, &host_addr, &host_addr_len, &request_timeout_ms);
  if (err)
    return err;

  OzoneAllocator* sockets_allocator = ozoneAllocatorCreate(0);
  OzonePollingEventVector events
      = ozoneVectorAllocate(sockets_allocator, OzonePollingEvent, OZONE_SOCKET_EVENT_POLLING_CAPACITY);
  SocketFdVector sockets_waiting = (SocketFdVector) { 0 };
  SocketFdVector sockets_processing = (SocketFdVector) { 0 };
  pthread_mutex_t sockets_write_lock = (pthread_mutex_t) { 0 };
  pthread_mutex_init(&sockets_write_lock, NULL);

  OzoneSocketWorkerVector workers = ozoneVectorAllocate(sockets_allocator, OzoneSocketWorker, config->workers);
  for (size_t worker_index = 0; worker_index < config->workers; worker_index++) {
    OzoneSocketWorker* worker = ozoneAllocatorReserveOne(sockets_allocator, OzoneSocketWorker);
    *worker = (OzoneSocketWorker) {
      .id = (int)worker_index + 1,
      .config = config,
      .sockets_allocator = sockets_allocator,
      .sockets_processing = &sockets_processing,
      .sockets_waiting = &sockets_waiting,
      .sockets_write_lock = &sockets_write_lock,
    };

    pthread_create(&worker->thread, NULL, (void* (*)(void*))ozoneSocketHandleWorker, worker);
    OzoneSocketWorkerVectorPush(sockets_allocator, &workers, worker);
  }

  struct sigaction signal_actions = { .sa_handler = &ozoneSocketShutdownSignalAction };
  sigaction(SIGINT, &signal_actions, NULL);
  sigaction(SIGTERM, &signal_actions, NULL);
  ozoneLogDebug("Listening for TCP connections on port %d", config->port);
  while (!ozone_socket_shutdown) {
    OzonePollingEventVectorClear(&events);
#ifdef OZONE_SOCKET_USE_KQUEUE
    events.length = (size_t)kevent(
        polling_fd,
        NULL,
        0,
        ozoneVectorBegin(&events),
        events.capacity,
        &(struct timespec) { .tv_sec = OZONE_SOCKET_POLLING_TIMEOUT_MS * 1000 });
#else
    events.length = epoll_wait(polling_fd, ozoneVectorBegin(&events), events.capacity, OZONE_SOCKET_POLLING_TIMEOUT_MS);
#endif

    OzonePollingEvent* event;
    ozoneVectorForEach(event, &events) {
#ifdef OZONE_SOCKET_USE_KQUEUE
      int event_fd = (int)event->ident;
#else
      if (event->events != EPOLLIN)
        continue;

      int event_fd = event->data.fd;
#endif
      if (!event_fd)
        continue;

      while (event_fd == listening_socket_fd) {
        int accepted_socket_fd = accept(listening_socket_fd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addr_len);
        if (accepted_socket_fd == -1)
          break;

        OzonePollingEvent accepted_event = (OzonePollingEvent) { 0 };
#ifdef OZONE_SOCKET_USE_KQUEUE
        EV_SET(&accepted_event, accepted_socket_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
        if (kevent(polling_fd, &accepted_event, 1, NULL, 0, NULL)) {
          ozoneLogError("Could not poll connection %d", accepted_socket_fd);
          close(accepted_socket_fd);
        }
#else
        accepted_event.data.fd = accepted_socket_fd;
        accepted_event.events = EPOLLIN | EPOLLET;
        if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, accepted_socket_fd, &accepted_event)) {
          ozoneLogError("Could not poll connection %d", accepted_socket_fd);
          close(accepted_socket_fd);
        }
#endif
      }

      if (event_fd == listening_socket_fd)
        continue;

      pthread_mutex_lock(&sockets_write_lock);
      int socket_reused = 0;
      SocketFd* socket_fd;
      ozoneVectorForEach(socket_fd, &sockets_processing) {
        if (event_fd == *socket_fd) {
          socket_reused = 1;
          break;
        }
      }

      int* reusable_socket = NULL;
      if (!socket_reused) {
        ozoneVectorForEach(socket_fd, &sockets_waiting) {
          if (!*socket_fd)
            reusable_socket = socket_fd;

          if (event_fd == *socket_fd) {
            socket_reused = 1;
            break;
          }
        }
      }

      if (!socket_reused && reusable_socket) {
        *reusable_socket = event_fd;
        socket_reused = 1;
      }

      if (!socket_reused)
        SocketFdVectorPush(sockets_allocator, &sockets_waiting, &event_fd);

      pthread_mutex_unlock(&sockets_write_lock);
    }
  }

  OzoneSocketWorker* worker;
  ozoneVectorForEach(worker, &workers) pthread_join(worker->thread, NULL);
  pthread_mutex_destroy(&sockets_write_lock);
  ozoneAllocatorDelete(sockets_allocator);
  close(polling_fd);
  close(listening_socket_fd);

  return 0;
}
