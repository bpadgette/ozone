#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define OZONE_SOCKET_DEFAULT_WORKER_CAPACITY 16
#define OZONE_SOCKET_IDLE_NANOSECONDS 10000
#define OZONE_SOCKET_POLLING_TIMEOUT_SECONDS 10
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192
#define OZONE_SOCKET_WORKER_CONNECTION_CAPACITY 64

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

OZONE_VECTOR_IMPLEMENT_API(OzoneSocketHandlerRef)

#define OZONE_SOCKET_WORKER_INIT 0
#define OZONE_SOCKET_WORKER_READ 1
#define OZONE_SOCKET_WORKER_HANDLING 2
#define OZONE_SOCKET_WORKER_SEND 3
#define OZONE_SOCKET_WORKER_CLOSE 4
#define OZONE_SOCKET_WORKER_CLEANUP 5

// todo: thread-safety
typedef struct OzoneSocketWorkerStruct {
  pthread_t thread;
  int pending;
  int connection_fds[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY];
  int connection_states[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY];
  const OzoneSocketHandlerRefVector* handler_pipeline;
  void* handler_context;
} OzoneSocketWorker;

OZONE_VECTOR_DECLARE_API(OzoneSocketWorker)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketWorker)

int ozone_socket_shutdown = 0;
void ozoneSocketShutdownSignalAction(int signum) {
  (void)signum;
  ozoneLogWarn("Received shutdown signal");
  ozone_socket_shutdown = 1;
}

// todo: thread-safety
void* ozoneSocketHandleWorker(OzoneSocketWorker* worker) {
  struct timespec idle = (struct timespec) { .tv_nsec = OZONE_SOCKET_IDLE_NANOSECONDS };

  OzoneSocketEvent events[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY] = { 0 };
  while (!ozone_socket_shutdown) {
    if (!worker->pending) {
      nanosleep(&idle, NULL);
      continue;
    }

    for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY; connection_index++) {
      int connection_fd = worker->connection_fds[connection_index];
      if (!connection_fd)
        continue;

      OzoneSocketEvent* event = &events[connection_index];
      switch (worker->connection_states[connection_index]) {
      case OZONE_SOCKET_WORKER_INIT: {
        ozoneLogDebug("Worker picked up connection %d", connection_fd);
        if (!event->allocator)
          event->allocator = ozoneAllocatorCreate(1024 + OZONE_SOCKET_REQUEST_CHUNK_SIZE);

        events[connection_index] = (OzoneSocketEvent) { .allocator = event->allocator };
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_READ;
      }
      case OZONE_SOCKET_WORKER_READ: {
        int recv_code = 0;
        char recv_buffer[OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1] = { 0 };
        recv_code = recv(connection_fd, recv_buffer, OZONE_SOCKET_REQUEST_CHUNK_SIZE, MSG_DONTWAIT);

        if (recv_code == 0) {
          ozoneLogDebug("Client closed connection %d", connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLOSE;
          break;
        }

        if (recv_code > 0) {
          ozoneVectorPushOzoneString(
              event->allocator,
              &event->raw_socket_request,
              ozoneStringFromBuffer(event->allocator, recv_buffer, (size_t)recv_code));

          if (recv_code < OZONE_SOCKET_REQUEST_CHUNK_SIZE)
            worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_HANDLING;

          break;
        }

        if (errno == EWOULDBLOCK || errno == EAGAIN)
          break;

        if (errno == EBADF || errno == ENOTCONN) {
          ozoneLogWarn(
              "Tried to read from what may be a previously closed connection, errno %d for file %d",
              errno,
              connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLEANUP;
          break;
        }

        if (errno == ECONNRESET || errno == ECONNREFUSED) {
          ozoneLogDebug("Client closed connection %d", connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLOSE;
          break;
        }

        ozoneLogError("Could not read part of TCP connection request, errno %d for file %d", errno, connection_fd);
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLOSE;
        break;
      }
      case OZONE_SOCKET_WORKER_HANDLING: {
        if (!ozoneVectorLength(&event->raw_socket_request)) {
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLOSE;
          break;
        }

        ozoneLogDebug("Handling event on connection %d", connection_fd);
        OzoneSocketHandlerRef* handler;
        ozoneVectorForEach(handler, worker->handler_pipeline) (*handler)(event, worker->handler_context);
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_SEND;
      }
      case OZONE_SOCKET_WORKER_SEND: {
        if (!ozoneVectorLength(&event->raw_socket_response)) {
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLOSE;
          break;
        }

        OzoneString* chunk;
        ozoneVectorForEach(chunk, &event->raw_socket_response) {
          int send_code = send(connection_fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk), 0);
          if (send_code == (int)ozoneStringLength(chunk))
            continue;

          if (errno == EAGAIN || errno == EWOULDBLOCK)
            continue;

          ozoneLogDebug("Could not send part of TCP connection response, errno %d for file %d", errno, connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLOSE;
          break;
        }

        if (worker->connection_states[connection_index] == OZONE_SOCKET_WORKER_SEND)
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLEANUP;

        break;
      }
      case OZONE_SOCKET_WORKER_CLOSE: {
        ozoneLogDebug("Closing connection %d", connection_fd);
        close(connection_fd);
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CLEANUP;
      }
      case OZONE_SOCKET_WORKER_CLEANUP: {
        ozoneLogDebug("Cleaning up connection %d for now", connection_fd);
        ozoneAllocatorClear(event->allocator);
        events[connection_index] = (OzoneSocketEvent) { .allocator = event->allocator };
        worker->pending--;
        worker->connection_fds[connection_index] = 0;
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_INIT;
      }
      }
    }
  }

  for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY; connection_index++) {
    if (events[connection_index].allocator)
      ozoneAllocatorDelete(events[connection_index].allocator);
  }

  return NULL;
}

int ozoneSocketServeTCP(OzoneSocketConfig* config, void* context) {
  size_t worker_pool_capacity = OZONE_SOCKET_DEFAULT_WORKER_CAPACITY;
  if (config->max_workers && config->max_workers < worker_pool_capacity) {
    worker_pool_capacity = config->max_workers;
  }

  struct sigaction signal_actions = { .sa_handler = &ozoneSocketShutdownSignalAction };
  sigaction(SIGINT, &signal_actions, NULL);
  sigaction(SIGTERM, &signal_actions, NULL);

  int listening_socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (listening_socket_fd == -1) {
    ozoneLogError("Failed to get AF_INET6 SOCK_STREAM socket file descriptor, returning EACCES");
    return EACCES;
  }

  // todo: review socket options, consider SO_LINGER
  const int socket_option_one = 1;
  setsockopt(listening_socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_one, sizeof(int));
  if (ioctl(listening_socket_fd, FIONBIO, &socket_option_one) < 0)
    ozoneLogWarn("Connection ioctl FIONBIO failed, socket may run in blocking mode");

  OzonePollingEvent listening_socket_event;

#ifdef OZONE_SOCKET_USE_KQUEUE
  struct timespec kqueue_timeout = (struct timespec) { .tv_sec = OZONE_SOCKET_POLLING_TIMEOUT_SECONDS };

  int polling_fd = kqueue();
  if (polling_fd == -1) {
    ozoneLogError("Failed to get kqueue handle, returning EACCES");
    close(listening_socket_fd);
    return EACCES;
  }

  EV_SET(&listening_socket_event, listening_socket_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

  if (kevent(polling_fd, &listening_socket_event, 1, NULL, 0, NULL) == -1) {
    ozoneLogError("Failed to add socket polling event to kqueue handle, returning EACCES");
    close(listening_socket_fd);
    close(polling_fd);
    return EACCES;
  }
#else
  int polling_fd = epoll_create1(0);
  if (polling_fd == -1) {
    ozoneLogError("Failed to get epoll file descriptor, returning EACCES");
    close(listening_socket_fd);
    return EACCES;
  }

  listening_socket_event.events = EPOLLIN;
  listening_socket_event.data.fd = listening_socket_fd;

  if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, listening_socket_fd, &listening_socket_event)) {
    ozoneLogError("Failed to add socket polling event to epoll, returning EACCES");
    close(listening_socket_fd);
    close(polling_fd);
    return EACCES;
  }
#endif

  struct sockaddr_in6 host_addr = { 0 };
  socklen_t host_addr_len = sizeof(host_addr);
  host_addr.sin6_family = AF_INET6;
  host_addr.sin6_port = htons(config->port);
  if (bind(listening_socket_fd, (struct sockaddr*)&host_addr, host_addr_len) != 0) {
    ozoneLogError("Bind failed, returning ECONNABORTED");
    return ECONNABORTED;
  }

  if (listen(listening_socket_fd, SOMAXCONN) != 0) {
    ozoneLogError("Listen on port %d failed", config->port);
    return ECONNABORTED;
  }

  ozoneLogDebug(
      "Listening for TCP connections on port %d with a %ld member handler_pipeline",
      config->port,
      ozoneVectorLength(&config->handler_pipeline));

  OzoneAllocator* socket_allocator = ozoneAllocatorCreate(0);
  OzonePollingEventVector events = ozoneVectorAllocate(
      socket_allocator, OzonePollingEvent, OZONE_SOCKET_WORKER_CONNECTION_CAPACITY * worker_pool_capacity);
  OzoneSocketWorkerVector worker_pool = ozoneVectorAllocate(socket_allocator, OzoneSocketWorker, worker_pool_capacity);
  worker_pool.length = 1;
  OzoneSocketWorker* worker = &ozoneVectorAt(&worker_pool, 0);
  worker->handler_context = context;
  worker->handler_pipeline = &config->handler_pipeline;
  pthread_create(&worker->thread, NULL, (void* (*)(void*))ozoneSocketHandleWorker, worker);

  struct timespec idle = (struct timespec) { .tv_nsec = OZONE_SOCKET_IDLE_NANOSECONDS };
  while (!ozone_socket_shutdown) {
    ozoneVectorClearOzonePollingEvent(&events);
#ifdef OZONE_SOCKET_USE_KQUEUE
    events.length = (size_t)kevent(polling_fd, NULL, 0, ozoneVectorBegin(&events), events.capacity, &kqueue_timeout);
#else
    events.length = epoll_wait(
        polling_fd, ozoneVectorBegin(&events), events.capacity, OZONE_SOCKET_POLLING_TIMEOUT_SECONDS * 1000);
#endif

    if (ozone_socket_shutdown)
      break;

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
        break;

      while (event_fd == listening_socket_fd) {
        int accepted_socket_fd = accept(listening_socket_fd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addr_len);

        if (accepted_socket_fd == -1)
          break;

        int locked = 0;
        ozoneVectorForEach(worker, &worker_pool) {
          for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
               connection_index++) {
            if (worker->connection_fds[connection_index] == accepted_socket_fd) {
              ozoneLogWarn("Connection %d is locked", accepted_socket_fd);
              locked = 1;
              break;
            }
          }

          if (locked)
            break;
        }

        if (locked)
          continue;

        ozoneLogDebug("Accepted connection %d", accepted_socket_fd);

        OzonePollingEvent accepted_event = (OzonePollingEvent) { 0 };

#ifdef OZONE_SOCKET_USE_KQUEUE
        EV_SET(&accepted_event, accepted_socket_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
        if (kevent(polling_fd, &accepted_event, 1, NULL, 0, NULL)) {
          ozoneLogError("Could not poll connection %d", accepted_socket_fd);
          close(accepted_socket_fd);
        }
#else
        accepted_event.data.fd = accepted_socket_fd;
        accepted_event.events = EPOLLIN;

        if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, accepted_socket_fd, &accepted_event)) {
          ozoneLogError("Could not poll connection %d", accepted_socket_fd);
          close(accepted_socket_fd);
        }
#endif
      }

      if (event_fd == listening_socket_fd)
        continue;

      int locked = 0;
      while (!locked) {
        int pending_least = OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
        OzoneSocketWorker* accepted_worker = NULL;
        ozoneVectorForEach(worker, &worker_pool) {
          for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
               connection_index++) {
            if (worker->connection_fds[connection_index] == event_fd) {
              locked = 1;
              break;
            }
          }

          if (locked) {
            break;
          }

          int pending = worker->pending;
          if (pending < pending_least) {
            pending_least = pending;
            accepted_worker = worker;
          }
        }

        if (locked)
          break;

        if (!accepted_worker && ozoneVectorLength(&worker_pool) == worker_pool_capacity) {
          nanosleep(&idle, NULL);
          continue;
        }

        if (!accepted_worker) {
          accepted_worker = &ozoneVectorAt(&worker_pool, worker_pool.length++);
          accepted_worker->handler_context = context;
          accepted_worker->handler_pipeline = &config->handler_pipeline;
          pthread_create(&accepted_worker->thread, NULL, (void* (*)(void*))ozoneSocketHandleWorker, worker);
        }

        for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
             connection_index++) {
          if (accepted_worker->connection_fds[connection_index] == 0) {
            accepted_worker->connection_fds[connection_index] = event_fd;
            accepted_worker->pending++;
            locked = 1;
            ozoneLogDebug("Passing connection %d to worker", event_fd);
            break;
          }
        }
      }
    }
  }

  ozoneVectorForEach(worker, &worker_pool) {
    if (worker->thread)
      pthread_join(worker->thread, NULL);
  }

  ozoneAllocatorDelete(socket_allocator);
  close(polling_fd);
  close(listening_socket_fd);

  return 0;
}
