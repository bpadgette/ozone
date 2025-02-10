#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define OZONE_SOCKET_DEFAULT_WORKER_CAPACITY 32
#define OZONE_SOCKET_POLLING_TIMEOUT_SECONDS 8
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192
#define OZONE_SOCKET_WORKER_CONNECTION_CAPACITY 32
#define OZONE_SOCKET_WORKER_IDLE_NANOSECONDS 8192
#define OZONE_SOCKET_WORKER_IDLE_KEEP_ALIVE_CYCLES 5 * (100000000 / OZONE_SOCKET_WORKER_IDLE_NANOSECONDS)

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

#define OZONE_SOCKET_WORKER_CONNECTION_INIT 0
#define OZONE_SOCKET_WORKER_CONNECTION_READ 1
#define OZONE_SOCKET_WORKER_CONNECTION_HANDLING 2
#define OZONE_SOCKET_WORKER_CONNECTION_SEND 3
#define OZONE_SOCKET_WORKER_CONNECTION_CLOSE 4
#define OZONE_SOCKET_WORKER_CONNECTION_CLEANUP 5

typedef struct OzoneSocketWorkerStruct {
  size_t id;
  pthread_t thread;
  pthread_mutex_t thread_lock;
  int pending;
  int connection_fds[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY];
  size_t connection_send_chunk_index[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY];
  int connection_states[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY];
  void* handler_context;
  const OzoneSocketHandlerRefVector* handler_pipeline;
} OzoneSocketWorker;

OZONE_VECTOR_DECLARE_API(OzoneSocketWorker)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketWorker)

int ozone_socket_shutdown = 0;
void ozoneSocketShutdownSignalAction(int signum) {
  (void)signum;
  ozoneLogWarn("Received shutdown signal");
  ozone_socket_shutdown = 1;
}

void* ozoneSocketHandleWorker(OzoneSocketWorker* worker) {
  ozoneLogDebug("Worker %ld: Created", worker->id);
  struct timespec idle = (struct timespec) { .tv_nsec = OZONE_SOCKET_WORKER_IDLE_NANOSECONDS };
  size_t idle_cycles = 0;

  OzoneSocketEvent events[OZONE_SOCKET_WORKER_CONNECTION_CAPACITY] = { 0 };
  while (!ozone_socket_shutdown && worker->pending >= 0 && idle_cycles < OZONE_SOCKET_WORKER_IDLE_KEEP_ALIVE_CYCLES) {
    if (!worker->pending) {
      nanosleep(&idle, NULL);
      idle_cycles++;
      continue;
    }

    idle_cycles = 0;

    for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY; connection_index++) {
      int connection_fd = worker->connection_fds[connection_index];
      if (!connection_fd)
        continue;
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif
      switch (worker->connection_states[connection_index]) {
      case OZONE_SOCKET_WORKER_CONNECTION_INIT: {
        ozoneLogDebug("Worker %ld: Preparing to handle connection %d", worker->id, connection_fd);
        if (!events[connection_index].allocator)
          events[connection_index].allocator = ozoneAllocatorCreate(1024 + OZONE_SOCKET_REQUEST_CHUNK_SIZE);

        events[connection_index] = (OzoneSocketEvent) { .allocator = events[connection_index].allocator,
                                                        .context = worker->handler_context };
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_READ;
      }
      case OZONE_SOCKET_WORKER_CONNECTION_READ: {
        int recv_code = 0;
        char recv_buffer[OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1] = { 0 };
        recv_code = recv(connection_fd, recv_buffer, OZONE_SOCKET_REQUEST_CHUNK_SIZE, MSG_DONTWAIT);

        if (recv_code == 0) {
          ozoneLogDebug("Worker %ld: Client closed connection %d", worker->id, connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLOSE;
          break;
        }

        if (recv_code > 0) {
          ozoneVectorPushOzoneString(
              events[connection_index].allocator,
              &events[connection_index].raw_socket_request,
              ozoneStringFromBuffer(events[connection_index].allocator, recv_buffer, (size_t)recv_code));

          if (recv_code < OZONE_SOCKET_REQUEST_CHUNK_SIZE)
            worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_HANDLING;

          break;
        }

        if (errno == EWOULDBLOCK || errno == EAGAIN) {
          break;
        }

        if (errno == EBADF || errno == ENOTCONN) {
          ozoneLogWarn(
              "Worker %ld: Tried to read from what may be a previously closed connection, errno %d for file %d",
              worker->id,
              errno,
              connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLEANUP;
          break;
        }

        if (errno == ECONNRESET || errno == ECONNREFUSED) {
          ozoneLogDebug("Worker %ld: Client closed connection %d", worker->id, connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLOSE;
          break;
        }

        ozoneLogError(
            "Worker %ld: Could not read part of TCP connection request, errno %d for file %d",
            worker->id,
            errno,
            connection_fd);
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLOSE;
        break;
      }
      case OZONE_SOCKET_WORKER_CONNECTION_HANDLING: {
        if (!ozoneVectorLength(&events[connection_index].raw_socket_request)) {
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLOSE;
          break;
        }

        OzoneSocketHandlerRef* handler;
        ozoneVectorForEach(handler, worker->handler_pipeline) (*handler)(&events[connection_index]);
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_SEND;
      }
      case OZONE_SOCKET_WORKER_CONNECTION_SEND: {
        if (!ozoneVectorLength(&events[connection_index].raw_socket_response)) {
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLOSE;
          break;
        }

        while (worker->connection_send_chunk_index[connection_index]
               < ozoneVectorLength(&events[connection_index].raw_socket_response)) {

          OzoneString* chunk = &ozoneVectorAt(
              &events[connection_index].raw_socket_response, worker->connection_send_chunk_index[connection_index]++);

          int send_code = send(connection_fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk), MSG_DONTWAIT);
          if (send_code == (int)ozoneStringLength(chunk))
            break;

          if (errno == EAGAIN || errno == EWOULDBLOCK)
            break;

          ozoneLogDebug(
              "Worker %ld: Could not send part of TCP connection response, errno %d for file %d",
              worker->id,
              errno,
              connection_fd);
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLOSE;
        }

        if (worker->connection_send_chunk_index[connection_index]
                == ozoneVectorLength(&events[connection_index].raw_socket_response)
            && worker->connection_states[connection_index] == OZONE_SOCKET_WORKER_CONNECTION_SEND)
          worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLEANUP;

        break;
      }
      case OZONE_SOCKET_WORKER_CONNECTION_CLOSE: {
        ozoneLogDebug("Worker %ld: Closing connection %d", worker->id, connection_fd);
        close(connection_fd);
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_CLEANUP;
      }
      case OZONE_SOCKET_WORKER_CONNECTION_CLEANUP: {
        pthread_mutex_lock(&worker->thread_lock);
        worker->pending--;
        worker->connection_fds[connection_index] = 0;
        worker->connection_send_chunk_index[connection_index] = 0;
        worker->connection_states[connection_index] = OZONE_SOCKET_WORKER_CONNECTION_INIT;
        pthread_mutex_unlock(&worker->thread_lock);
        ozoneLogDebug(
            "Worker %ld: Cleaning up connection %d for now, worker has %d pending connections",
            worker->id,
            connection_fd,
            worker->pending);
        ozoneAllocatorClear(events[connection_index].allocator);
        events[connection_index] = (OzoneSocketEvent) { .allocator = events[connection_index].allocator };
      }
      }
    }
  }

#ifndef __clang__
#pragma GCC diagnostic pop
#endif

  pthread_mutex_lock(&worker->thread_lock);
  for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY; connection_index++) {
    if (events[connection_index].allocator)
      ozoneAllocatorDelete(events[connection_index].allocator);

    events[connection_index] = (OzoneSocketEvent) { 0 };
    worker->connection_fds[connection_index] = 0;
    worker->connection_states[connection_index] = 0;
  }

  worker->pending = -1;
  pthread_mutex_unlock(&worker->thread_lock);
  ozoneLogDebug("Worker %ld: Finished", worker->id);

  return NULL;
}

int ozoneSocketServeTCP(OzoneSocketConfig* config) {
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
  for (size_t worker_id = 0; worker_id < worker_pool.capacity; worker_id++) {
    worker_pool.elements[worker_id] = (OzoneSocketWorker) {
      .id = worker_id + 1,
      .handler_context = config->handler_context,
      .handler_pipeline = &config->handler_pipeline,
    };
    pthread_mutex_init(&worker_pool.elements[worker_id].thread_lock, NULL);
  }

  struct timespec idle = (struct timespec) { .tv_nsec = OZONE_SOCKET_WORKER_IDLE_NANOSECONDS };
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
      while (!locked && !ozone_socket_shutdown) {
        int pending_least = OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
        OzoneSocketWorker* accepted_worker = NULL;
        OzoneSocketWorker* backup_offline_worker = NULL;
        OzoneSocketWorker* worker;
        ozoneVectorForEach(worker, &worker_pool) {
          if (pthread_mutex_trylock(&worker->thread_lock))
            continue;

          if (worker->pending == -1) {
            if (worker->thread)
              pthread_join(worker->thread, NULL);

            worker->thread = 0;
            worker->pending = 0;
          }

          if (!worker->thread) {
            if (!backup_offline_worker)
              backup_offline_worker = worker;

            pthread_mutex_unlock(&worker->thread_lock);
            continue;
          }

          for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
               connection_index++) {
            if (worker->connection_fds[connection_index] == event_fd) {
              locked = 1;
              break;
            }
          }

          if (!locked && worker->pending < pending_least) {
            pending_least = worker->pending;
            accepted_worker = worker;
          }
          pthread_mutex_unlock(&worker->thread_lock);
          if (locked)
            break;
        }

        if (locked)
          break;

        if (!accepted_worker && backup_offline_worker) {
          accepted_worker = backup_offline_worker;
        }

        if (!accepted_worker && ozoneVectorLength(&worker_pool) < worker_pool_capacity) {
          accepted_worker = &ozoneVectorAt(&worker_pool, worker_pool.length++);
        }

        if (!accepted_worker) {
          nanosleep(&idle, NULL);
          continue;
        }

        pthread_mutex_lock(&accepted_worker->thread_lock);
        for (size_t connection_index = 0; connection_index < OZONE_SOCKET_WORKER_CONNECTION_CAPACITY;
             connection_index++) {
          if (!accepted_worker->connection_fds[connection_index]) {
            accepted_worker->connection_fds[connection_index] = event_fd;
            accepted_worker->pending++;
            ozoneLogDebug("Passing connection %d to worker %ld", event_fd, accepted_worker->id);
            locked = 1;
            break;
          }
        }
        pthread_mutex_unlock(&accepted_worker->thread_lock);

        if (!accepted_worker->thread)
          pthread_create(&accepted_worker->thread, NULL, (void* (*)(void*))ozoneSocketHandleWorker, accepted_worker);
      }
    }
  }

  OzoneSocketWorker* worker;
  ozoneVectorForEach(worker, &worker_pool) {
    pthread_mutex_destroy(&worker->thread_lock);
    if (worker->thread)
      pthread_join(worker->thread, NULL);
  }

  ozoneAllocatorDelete(socket_allocator);
  close(polling_fd);
  close(listening_socket_fd);

  return 0;
}
