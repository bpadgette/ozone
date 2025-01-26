#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <unistd.h>

#if defined(__APPLE__) || defined(__FreeBSD__)
#define OZONE_SOCKET_USE_KQUEUE 1
#endif

#if OZONE_SOCKET_USE_KQUEUE
#include <sys/event.h>
typedef struct kevent OzoneSocketPollEvent;
#define ozoneSocketPollEventFile(_event_) ((int)((_event_)->ident))

#else
#include <sys/epoll.h>
typedef struct epoll_event OzoneSocketPollEvent;
#define ozoneSocketPollEventFile(_event_) ((_event_)->data.fd)

#endif

#define OZONE_SOCKET_POLLING_TIMEOUT_SECONDS 60
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192

OZONE_VECTOR_DECLARE_API(OzoneSocketPollEvent)
OZONE_VECTOR_IMPLEMENT_API(OzoneSocketPollEvent)

OZONE_VECTOR_IMPLEMENT_API(OzoneSocketHandlerRef)

int ozone_socket_shutdown = 0;
void ozoneSocketSignalAction(int signum) {
  if (signum == SIGINT) {
    ozoneLogError("Received SIGINT, will shutdown");
    ozone_socket_shutdown = 1;
  }
}

int ozoneSocketServeTCP(OzoneSocketConfig* config, void* context) {
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

  OzoneSocketPollEvent listening_socket;

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

  size_t connection_pool_capacity = SOMAXCONN - 1;
  if (config->max_connections && config->max_connections < connection_pool_capacity) {
    connection_pool_capacity = config->max_connections;
  }

  OzoneAllocator* handler_allocator = ozoneAllocatorCreate(1024 + OZONE_SOCKET_REQUEST_CHUNK_SIZE);
  OzoneAllocator* socket_allocator = ozoneAllocatorCreate(1024);
#ifdef OZONE_SOCKET_USE_KQUEUE
  OzoneSocketPollEventVector connection_pool
      = ozoneVectorAllocate(socket_allocator, OzoneSocketPollEvent, connection_pool_capacity);
  connection_pool.length = connection_pool.capacity;
  ozoneLogDebug("Connection pool will handle up to %ld connections", ozoneVectorLength(&connection_pool));
#else
  OzoneSocketPollEventVector connection_pool
      = ozoneVectorAllocate(socket_allocator, OzoneSocketPollEvent, connection_pool_capacity + 1);
  ozoneVectorPushOzoneSocketPollEvent(socket_allocator, &connection_pool, &listening_socket);
  connection_pool.length = connection_pool.capacity;
  ozoneLogDebug("Connection pool will handle up to %ld connections", ozoneVectorLength(&connection_pool) - 1);
#endif

  while (!ozone_socket_shutdown) {

#ifdef OZONE_SOCKET_USE_KQUEUE
    int events_count = kevent(
        polling_fd, NULL, 0, ozoneVectorBegin(&connection_pool), ozoneVectorLength(&connection_pool), &kqueue_timeout);
    size_t stop_index = events_count;
#else
    epoll_wait(
        polling_fd,
        ozoneVectorBegin(&connection_pool),
        ozoneVectorLength(&connection_pool),
        OZONE_SOCKET_POLLING_TIMEOUT_SECONDS * 1000);
    size_t stop_index = ozoneVectorLength(&connection_pool);
#endif
    for (size_t connection_index = 0; connection_index < stop_index && !ozone_socket_shutdown; connection_index++) {
      OzoneSocketPollEvent* connection = &ozoneVectorAt(&connection_pool, connection_index);
      int connection_fd = ozoneSocketPollEventFile(connection);
      if (!connection_fd)
        continue;

      for (; connection_fd == socket_fd;) {
        OzoneSocketPollEvent* accepted_connection = NULL;
        OzoneSocketPollEvent* existing_connection;
        ozoneVectorForEach(existing_connection, &connection_pool) {
          if (!ozoneSocketPollEventFile(existing_connection)) {
            accepted_connection = existing_connection;
            break;
          }
        }

        if (!accepted_connection) {
          ozoneLogDebug("Waiting to accept connection, all connections in pool are occupied");
          break;
        }

        int accepted_socket_fd = accept(
            ozoneSocketPollEventFile(&listening_socket), (struct sockaddr*)&host_addr, (socklen_t*)&host_addr_len);
        if (accepted_socket_fd == -1)
          break;

        ozoneVectorForEach(existing_connection, &connection_pool) {
          if (ozoneSocketPollEventFile(existing_connection) == accepted_socket_fd) {
            accepted_connection = existing_connection;
            break;
          }
        }

        ozoneLogDebug("Accepted new connection %d", accepted_socket_fd);

#ifdef OZONE_SOCKET_USE_KQUEUE
        EV_SET(accepted_connection, accepted_socket_fd, EVFILT_READ, EV_ADD, 0, 0, 0);
        if (kevent(polling_fd, accepted_connection, 1, NULL, 0, NULL)) {
          ozoneLogError("Could not poll connection %d", accepted_socket_fd);
          close(accepted_socket_fd);
          *accepted_connection = (OzoneSocketPollEvent) { 0 };
          break;
        }
#else
        accepted_connection->data.fd = accepted_socket_fd;
        accepted_connection->events = EPOLLIN;

        if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, accepted_socket_fd, accepted_connection)) {
          ozoneLogError("Could not poll connection %d", accepted_socket_fd);
          close(accepted_socket_fd);
          *accepted_connection = (OzoneSocketPollEvent) { 0 };
          break;
        }
#endif

        ozoneLogDebug("Polling connection %d", accepted_socket_fd);
      }

      if (connection_fd == socket_fd)
        continue;

      ozoneAllocatorClear(handler_allocator);
      OzoneSocketEvent event = { .allocator = handler_allocator };

      int close_connection = 0;
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
            // This is against the spirit of non-blocking sockets, handler event management will come in another feature
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
        ozoneVectorForEach(handler, &config->handler_pipeline) (*handler)(&event, context);

        OzoneString* chunk;
        ozoneVectorForEach(chunk, &event.raw_socket_response)
            send(connection_fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk), MSG_NOSIGNAL);
      }

      if (close_connection) {
        ozoneLogDebug("Closing server connection %d", connection_fd);
        close(connection_fd);

#ifndef OZONE_SOCKET_USE_KQUEUE
        // This could be unnecessary following close(connection_fd), but best to be safe across epoll implementations
        epoll_ctl(polling_fd, EPOLL_CTL_DEL, connection_fd, NULL);
#endif

        ozoneVectorForEach(connection, &connection_pool) {
          if (ozoneSocketPollEventFile(connection) == connection_fd)
            *connection = (OzoneSocketPollEvent) { 0 };
        }
      }
    }
  }

  OzoneSocketPollEvent* connection;
  ozoneVectorForEach(connection, &connection_pool) close(ozoneSocketPollEventFile(connection));

  ozoneAllocatorDelete(handler_allocator);
  ozoneAllocatorDelete(socket_allocator);
  close(polling_fd);
  close(socket_fd);

  return 0;
}
