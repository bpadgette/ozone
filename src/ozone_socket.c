#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 8192

OZONE_VECTOR_IMPLEMENT_API(OzoneSocketHandlerRef)

int ozone_socket_shutdown = 0;
void ozoneSocketSignalAction(int signum) {
  if (signum == SIGINT) {
    ozoneLogError("Received SIGINT, will shutdown");
    ozone_socket_shutdown = 1;
  }
}

int ozoneSocketServeTCP(OzoneSocketConfig* config, void* context) {
  int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    ozoneLogError("Failed to get AF_INET6 SOCK_STREAM socket file descriptor, returning EACCES");
    return EACCES;
  }

  struct sockaddr_in6 host_addr = { 0 };
  int host_addrlen = sizeof(host_addr);
  host_addr.sin6_family = AF_INET6;
  host_addr.sin6_port = htons(config->port);

  // todo: review socket options, consider SO_LINGER
  const int socket_option_one = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_one, sizeof(int));
  setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &socket_option_one, sizeof(int));
  setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPIDLE, &socket_option_one, sizeof(int));
  setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPINTVL, &socket_option_one, sizeof(int));
  const int socket_option_ten = 10;
  setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPCNT, &socket_option_ten, sizeof(int));

  if (bind(socket_fd, (struct sockaddr*)&host_addr, host_addrlen) != 0 || listen(socket_fd, SOMAXCONN) != 0) {
    ozoneLogError("Connection aborted, returning ECONNABORTED");
    return ECONNABORTED;
  }

  ozoneLogDebug(
      "Listening for TCP connections on port %d with a %ld member handler_pipeline",
      config->port,
      ozoneVectorLength(&config->handler_pipeline));
  OzoneAllocator* handler_allocator = ozoneAllocatorCreate(1024 + OZONE_SOCKET_REQUEST_CHUNK_SIZE);

  struct sigaction signal_actions = { .sa_handler = &ozoneSocketSignalAction };
  sigaction(SIGINT, &signal_actions, NULL);

  while (!ozone_socket_shutdown) {
    int accepted_socket_fd = accept(socket_fd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addrlen);

    if (accepted_socket_fd < 0) {
      ozoneLogError("Could not accept connection");
      continue;
    }

    ozoneAllocatorClear(handler_allocator);

    OzoneSocketEvent event = {
      .allocator = handler_allocator,
      .raw_socket_request = ((OzoneStringVector) { 0 }),
      .raw_socket_response = ((OzoneStringVector) { 0 }),
    };

    int read_status = 0;
    do {
      size_t capacity = OZONE_SOCKET_REQUEST_CHUNK_SIZE + 1;
      OzoneByteVector vector = (OzoneByteVector) {
        .capacity = capacity,
        .elements = ozoneAllocatorReserveMany(handler_allocator, char, capacity),
        .length = capacity,
      };

      read_status = read(accepted_socket_fd, vector.elements, vector.capacity);

      ozoneLogDebug("read accepted_socket_fd returned %d", read_status);
      if (read_status < 0)
        ozoneLogError(
            "Could not read part of TCP connection request, read accepted_socket_fd returned %d", read_status);

      if (read_status > 0) {
        vector.length = (size_t)read_status;
        OzoneString string = (OzoneString) {
          .vector = vector,
        };

        ozoneVectorPushOzoneString(handler_allocator, &event.raw_socket_request, &string);
      }
    } while (read_status >= OZONE_SOCKET_REQUEST_CHUNK_SIZE);

    OzoneSocketHandlerRef* handler;
    ozoneVectorForEach(handler, &config->handler_pipeline) { (*handler)(&event, context); }

    int write_status = 0;
    OzoneString* chunk;
    ozoneVectorForEach(chunk, &event.raw_socket_response) {
      write_status = write(accepted_socket_fd, ozoneStringBuffer(chunk), ozoneStringLength(chunk));
      if (write_status == -1) {
        ozoneLogError(
            "Could not write part of TCP connection response, write accepted_socket_fd returned %d", write_status);
        break;
      }
    }

    close(accepted_socket_fd);
  }

  ozoneAllocatorDelete(handler_allocator);

  return 0;
}
