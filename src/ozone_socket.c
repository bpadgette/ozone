#include "ozone_socket.h"

#include "ozone_log.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <sys/socket.h>
#include <unistd.h>

#define OZONE_SOCKET_REQUEST_CHUNK_SIZE 1024
#define OZONE_SOCKET_INITIAL_ALLOCATION 32 * 1024

int ozone_socket_shutdown = 0;
void ozoneSocketSignalAction(int signum) {
  if (signum == SIGINT) {
    ozoneLogInfo("Received SIGINT, will shutdown");
    ozone_socket_shutdown = 1;
    fflush(stdout);
  }
}

int ozoneSocketServeTCP(OzoneSocketConfigT config) {
  int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    ozoneLogError("Failed to get AF_INET6 SOCK_STREAM socket file descriptor, returning EACCES");
    return EACCES;
  }

  struct sockaddr_in6 host_addr = { 0 };
  int host_addrlen = sizeof(host_addr);
  host_addr.sin6_family = AF_INET6;
  host_addr.sin6_port = htons(config.port);

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

  ozoneLogDebug("Listening for TCP connections on port %d with a %ld member handler_pipeline", config.port,
      config.handler_pipeline_count);
  OzoneAllocatorT* handler_allocator = ozoneAllocatorCreate(OZONE_SOCKET_INITIAL_ALLOCATION);

  struct sigaction signal_actions = { .sa_handler = &ozoneSocketSignalAction };
  sigaction(SIGINT, &signal_actions, NULL);

  while (!ozone_socket_shutdown) {
    int accepted_socket_fd = accept(socket_fd, (struct sockaddr*)&host_addr, (socklen_t*)&host_addrlen);

    if (accepted_socket_fd < 0) {
      ozoneLogError("Could not accept connection");
      continue;
    }

    ozoneAllocatorClear(handler_allocator);
    OzoneSocketChunkT request_chunks = { 0 };
    OzoneSocketChunkT* current_chunk = &request_chunks;
    int read_status = 0;
    do {
      ozoneLogTrace("read accepted_socket_fd returned %d", read_status);
      if (read_status == -1) {
        ozoneLogError(
            "Could not read part of TCP connection request, read accepted_socket_fd returned %d", read_status);
        break;
      }

      if (read_status != 0 && (size_t)read_status < current_chunk->length) {
        current_chunk->length = (size_t)read_status;
        break;
      }

      if (current_chunk->length) {
        current_chunk->next = ozoneAllocatorReserveOne(handler_allocator, OzoneSocketChunkT);
        current_chunk = current_chunk->next;
        *current_chunk = (OzoneSocketChunkT) { 0 };
      }

      current_chunk->buffer = ozoneAllocatorReserveMany(handler_allocator, char, OZONE_SOCKET_REQUEST_CHUNK_SIZE);
      current_chunk->length = OZONE_SOCKET_REQUEST_CHUNK_SIZE;
    } while ((read_status = read(accepted_socket_fd, current_chunk->buffer, current_chunk->length)));

    OzoneSocketContextT handler_arg = {
      .allocator = handler_allocator,
      .raw_request = &request_chunks,
      .raw_response = NULL,
      .application = config.application,
    };

    for (size_t handler_index = 0; handler_index < config.handler_pipeline_count; handler_index++) {
      config.handler_pipeline[handler_index](&handler_arg);
    }

    int write_status = 0;
    current_chunk = handler_arg.raw_response;
    while (current_chunk) {
      write_status = write(accepted_socket_fd, current_chunk->buffer, current_chunk->length);
      if (write_status == -1) {
        ozoneLogError(
            "Could not write part of TCP connection response, write accepted_socket_fd returned %d", write_status);
        break;
      }

      current_chunk = current_chunk->next;
    };

    close(accepted_socket_fd);
  }

  ozoneAllocatorDelete(handler_allocator);

  return 0;
}
