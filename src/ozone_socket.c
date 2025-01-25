#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define OZONE_SOCKET_POLLING_TIMEOUT_MS 60 * 1000
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192

typedef struct epoll_event OzoneSocketPollEvent;
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

  OzoneSocketPollEvent listening_socket;
  int polling_fd = epoll_create1(0);
  if (polling_fd == -1) {
    ozoneLogError("Failed to get epoll file descriptor, returning EACCES");
    close(socket_fd);
    return EACCES;
  }

  listening_socket.events = EPOLLIN;
  listening_socket.data.fd = socket_fd;

  if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, socket_fd, &listening_socket)) {
    ozoneLogError("Failed to add epoll file descriptor to epoll, returning EACCES");
    close(socket_fd);
    close(polling_fd);
    return EACCES;
  }

  // todo: review socket options, consider SO_LINGER
  const int socket_option_one = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_one, sizeof(int));

  struct timeval socket_option_recv_timeout;
  socket_option_recv_timeout.tv_sec = 1;
  socket_option_recv_timeout.tv_usec = 0;
  setsockopt(
      socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&socket_option_recv_timeout, sizeof(socket_option_recv_timeout));

  if (ioctl(socket_fd, FIONBIO, &socket_option_one) < 0)
    ozoneLogWarn("Connection ioctl FIONBIO failed, socket may run in blocking mode");

  struct sockaddr_in6 host_addr = { 0 };
  int host_addrlen = sizeof(host_addr);
  host_addr.sin6_family = AF_INET6;
  host_addr.sin6_port = htons(config->port);
  if (bind(socket_fd, (struct sockaddr*)&host_addr, host_addrlen) != 0) {
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
  OzoneSocketPollEventVector connection_pool
      = ozoneVectorAllocate(socket_allocator, OzoneSocketPollEvent, connection_pool_capacity + 1);
  ozoneVectorPushOzoneSocketPollEvent(socket_allocator, &connection_pool, &listening_socket);

  // poll descriptors are zero-initialized, so lets grow the connection_pool vector to its full capacity
  connection_pool.length = connection_pool.capacity;
  ozoneLogDebug("Connection pool will handle up to %ld connections", connection_pool.length);

  while (!ozone_socket_shutdown) {
    int events_count = epoll_wait(
        polling_fd,
        ozoneVectorBegin(&connection_pool),
        ozoneVectorLength(&connection_pool),
        OZONE_SOCKET_POLLING_TIMEOUT_MS);

    if (!events_count)
      continue;

    for (size_t connection_index = 0; connection_index < ozoneVectorLength(&connection_pool); connection_index++) {
      OzoneSocketPollEvent* connection = &ozoneVectorAt(&connection_pool, connection_index);
      int connection_fd = connection->data.fd;
      if (!connection_fd)
        continue;

      if (connection_fd == listening_socket.data.fd) {
        for (;;) {
          OzoneSocketPollEvent* accepted_connection = NULL;
          OzoneSocketPollEvent* existing_connection;
          ozoneVectorForEach(existing_connection, &connection_pool) {
            if (!existing_connection->data.fd) {
              accepted_connection = existing_connection;
              break;
            }
          }

          if (!accepted_connection)
            break;

          int accepted_socket_fd = accept(connection_fd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addrlen);
          if (accepted_socket_fd == -1)
            break;

          ozoneLogDebug("Accepted new connection %d", accepted_socket_fd);

          accepted_connection->data.fd = accepted_socket_fd;
          accepted_connection->events = EPOLLIN;

          if (epoll_ctl(polling_fd, EPOLL_CTL_ADD, accepted_socket_fd, accepted_connection)) {
            ozoneLogError("Could not poll connection %d", accepted_socket_fd);
            close(accepted_socket_fd);
            *accepted_connection = (OzoneSocketPollEvent) { 0 };
            break;
          }

          ozoneLogDebug("Polling connection %d", accepted_socket_fd);
        }

        continue;
      }

      OzoneSocketEvent event = { .allocator = handler_allocator };
      ozoneAllocatorClear(event.allocator);

      int close_connection = 0;
      int recv_code = 0;
      do {
        OzoneString* string = ozoneAllocatorReserveOne(event.allocator, OzoneString);
        string->vector = ozoneVectorAllocate(event.allocator, OzoneByte, OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1);

        recv_code = recv(connection_fd, ozoneStringBuffer(string), OZONE_SOCKET_REQUEST_CHUNK_SIZE, 0);
        if (recv_code < 0) {
          if (errno == ECONNRESET) {
            // Closing during read is common enough to handle silently
            close_connection = 1;
            break;
          }

          ozoneLogError("Could not read part of TCP connection request, errno %d for file %d", errno, connection_fd);
          close_connection = 1;
          break;
        }

        if (recv_code == 0) {
          ozoneLogDebug("Client closed connection %d", connection_fd);
          close_connection = 1;
          break;
        }

        string->vector.length = (size_t)recv_code;
        ozoneVectorPushOzoneString(event.allocator, &event.raw_socket_request, string);
      } while (recv_code >= OZONE_SOCKET_REQUEST_CHUNK_SIZE);

      if (!close_connection && ozoneVectorLength(&event.raw_socket_request)) {
        ozoneLogDebug("Handling event on connection %d", connection_fd);
        OzoneSocketHandlerRef* handler;
        ozoneVectorForEach(handler, &config->handler_pipeline) { (*handler)(&event, context); }

        OzoneString* chunk;
        ozoneVectorForEach(chunk, &event.raw_socket_response)
            send(connection_fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk), MSG_NOSIGNAL);
      }

      if (close_connection) {
        ozoneLogDebug("Closing server connection %d", connection_fd);
        epoll_ctl(polling_fd, EPOLL_CTL_DEL, connection_fd, NULL);
        close(connection_fd);
        *connection = (OzoneSocketPollEvent) { 0 };
        break;
      }
    }
  }

  OzoneSocketPollEvent* connection;
  ozoneVectorForEach(connection, &connection_pool) {
    if (connection->data.fd)
      close(connection->data.fd);
  }

  ozoneAllocatorDelete(handler_allocator);
  ozoneAllocatorDelete(socket_allocator);
  close(polling_fd);
  close(socket_fd);

  return 0;
}
