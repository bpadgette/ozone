#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#define OZONE_SOCKET_POLLING_TIMEOUT_MS 60 * 1000
#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192

OZONE_VECTOR_IMPLEMENT_API(OzoneSocketPollDescriptor)
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

  OzoneSocketPollDescriptor listening_socket = (OzoneSocketPollDescriptor) { .fd = socket_fd, .events = POLLIN };

  ozoneLogDebug(
      "Listening for TCP connections on port %d with a %ld member handler_pipeline",
      config->port,
      ozoneVectorLength(&config->handler_pipeline));

  OzoneAllocator* socket_allocator = ozoneAllocatorCreate(1024);
  OzoneAllocator* handler_allocator = ozoneAllocatorCreate(1024 + OZONE_SOCKET_REQUEST_CHUNK_SIZE);

  size_t connection_pool_capacity = SOMAXCONN;
  if (config->max_connections && config->max_connections < connection_pool_capacity) {
    connection_pool_capacity = config->max_connections;
  }

  OzoneSocketPollDescriptorVector connection_pool
      = ozoneVectorAllocate(socket_allocator, OzoneSocketPollDescriptor, connection_pool_capacity + 1);
  ozoneVectorPushOzoneSocketPollDescriptor(socket_allocator, &connection_pool, &listening_socket);

  // poll descriptors are zero-initialized, so lets grow the connection_pool vector to its full capacity
  connection_pool.length = connection_pool.capacity;
  ozoneLogDebug("Connection pool will handle up to %ld connections", connection_pool.length - 1);

  while (!ozone_socket_shutdown) {
    int poll_code = poll(connection_pool.elements, connection_pool.length, OZONE_SOCKET_POLLING_TIMEOUT_MS);
    if (poll_code == 0) {
      // Note: poll timeout, this could be a good time to block to execute scheduled jobs etc
      continue;
    }

    if (poll_code < 0) {
      ozoneLogError("Connection pool poll failed, shutting down socket");
      break;
    }

    for (size_t connection_index = 0; connection_index < connection_pool.length; connection_index++) {
      OzoneSocketPollDescriptor* connection = &ozoneVectorAt(&connection_pool, connection_index);
      if (!connection->fd)
        continue;

      if (connection->fd == listening_socket.fd) {
        ozoneLogDebug("Accepting new connections on listening socket %d", connection->fd);
        int accepted_socket_fd = 0;
        while (accepted_socket_fd != -1) {
          OzoneSocketPollDescriptor* accepted_connection = NULL;
          OzoneSocketPollDescriptor* existing_connection;
          ozoneVectorForEach(existing_connection, &connection_pool) {
            if (!existing_connection->fd) {
              accepted_connection = existing_connection;
              break;
            }
          }

          if (!accepted_connection)
            // no space in the connection pool, do not accept
            break;

          accepted_socket_fd = accept(connection->fd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addrlen);
          if (accepted_socket_fd != -1) {
            ozoneLogDebug("Accepted new connection %d", accepted_socket_fd);
            *accepted_connection = (OzoneSocketPollDescriptor) { .fd = accepted_socket_fd, .events = POLLIN };
          }
        }

        // this is the listening socket, continue to the next to handle events
        continue;
      }

      OzoneSocketEvent event = { .allocator = handler_allocator };
      ozoneAllocatorClear(event.allocator);

      int close_connection = 0;
      int recv_code = 0;
      do {
        OzoneString* string = ozoneAllocatorReserveOne(event.allocator, OzoneString);

        size_t capacity = OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1;
        string->vector = (OzoneByteVector) {
          .capacity = capacity,
          .elements = ozoneAllocatorReserveMany(event.allocator, char, capacity),
        };

        recv_code = recv(connection->fd, string->vector.elements, capacity - 1, 0);
        if (recv_code < 0) {
          if (errno == EWOULDBLOCK)
            break;

          if (errno == ECONNRESET) {
            // Closing during read is common enough to handle silently
            close_connection = 1;
            break;
          }

          ozoneLogError("Could not read part of TCP connection request, errno %d for file %d", errno, connection->fd);
          close_connection = 1;
          break;
        }

        if (recv_code == 0) {
          ozoneLogDebug("Client closed connection %d", connection->fd);
          close_connection = 1;
          break;
        }

        string->vector.length = (size_t)recv_code;
        ozoneVectorPushOzoneString(event.allocator, &event.raw_socket_request, string);
      } while (recv_code >= OZONE_SOCKET_REQUEST_CHUNK_SIZE);

      if (!close_connection && ozoneVectorLength(&event.raw_socket_request)) {
        ozoneLogDebug("Handling event on connection %d", connection->fd);
        OzoneSocketHandlerRef* handler;
        ozoneVectorForEach(handler, &config->handler_pipeline) { (*handler)(&event, context); }

        OzoneString* chunk;
        ozoneVectorForEach(chunk, &event.raw_socket_response)
            send(connection->fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk), MSG_NOSIGNAL);
      }

      if (close_connection) {
        ozoneLogDebug("Closing server connection %d", connection->fd);
        close(connection->fd);
        *connection = (OzoneSocketPollDescriptor) { 0 };
        break;
      }
    }
  }

  OzoneSocketPollDescriptor* connection;
  ozoneVectorForEach(connection, &connection_pool) {
    if (connection->fd)
      close(connection->fd);
  }

  ozoneAllocatorDelete(handler_allocator);
  ozoneAllocatorDelete(socket_allocator);

  return 0;
}
