#include "ozone_socket.h"

#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include "ozone_log.h"

int ozoneSocketServeTCP(OzoneSocketConfigT config)
{
  int socket_fd = socket(AF_INET6, SOCK_STREAM, 0);
  if (socket_fd == -1)
  {
    ozoneLogError("Failed to get AF_INET6 SOCK_STREAM socket file descriptor, returning EACCES");
    return EACCES;
  }

  struct sockaddr_in6 host_addr = {0};
  int host_addrlen = sizeof(host_addr);
  host_addr.sin6_family = AF_INET6;
  host_addr.sin6_port = htons(config.port);

  const int socket_option_one = 1;
  setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option_one, sizeof(int));
  setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE, &socket_option_one, sizeof(int));
  setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPIDLE, &socket_option_one, sizeof(int));
  setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPINTVL, &socket_option_one, sizeof(int));
  const int socket_option_ten = 10;
  setsockopt(socket_fd, IPPROTO_TCP, TCP_KEEPCNT, &socket_option_ten, sizeof(int));

  if (bind(socket_fd, (struct sockaddr *)&host_addr, host_addrlen) != 0 || listen(socket_fd, SOMAXCONN) != 0)
  {
    ozoneLogError("Connection aborted, returning ECONNABORTED");
    return ECONNABORTED;
  }

  ozoneLogDebug("Listening for TCP connections on port %d with a %ld member handler_pipeline", config.port, config.handler_pipeline_length);
  OzoneAllocatorT *handler_allocator = ozoneAllocatorCreate(OZONE_SOCKET_INITIAL_ALLOCATION);
  for (;;)
  {
    int accepted_socket_fd = accept(
        socket_fd,
        (struct sockaddr *)&host_addr,
        (socklen_t *)&host_addrlen);

    if (accepted_socket_fd < 0)
    {
      ozoneLogError("Could not accept connection");
      continue;
    }

    ozoneAllocatorClear(handler_allocator);
    OzoneSocketRequestT *request = ozoneSocketRequestCreate(handler_allocator, OZONE_SOCKET_REQUEST_MAX_LENGTH);

    long int bytes_read = 0;
    long int read_status = 0;
    do
    {
      long int bytes_readable = (bytes_read + OZONE_SOCKET_REQUEST_CHUNK_LENGTH) > OZONE_SOCKET_REQUEST_MAX_LENGTH
                                    ? OZONE_SOCKET_REQUEST_MAX_LENGTH - bytes_read
                                    : OZONE_SOCKET_REQUEST_CHUNK_LENGTH;

      read_status = bytes_readable > 0
                        ? read(accepted_socket_fd, bytes_read + request->data, bytes_readable)
                        : -2;

      if (read_status > 0 && read_status < bytes_readable)
        read_status = 0;
      else if (read_status == bytes_readable)
        bytes_read += bytes_readable;
      else if (read_status == -1)
        ozoneLogError(
            "Could not read (part of) a request; internal error, %ld bytes read",
            bytes_read);
      else if (read_status == -2)
        ozoneLogError(
            "Could not read (part of) a request; out of memory, %ld bytes read, OZONE_SOCKET_REQUEST_MAX_LENGTH %d",
            bytes_read, OZONE_SOCKET_REQUEST_MAX_LENGTH);
    } while (read_status > 0);

    int skip_write = 0;
    OzoneSocketHandlerParameterT handler_arg = {.request = request};
    for (size_t handler_index = 0; handler_index < config.handler_pipeline_length; handler_index++)
    {
      int error = config.handler_pipeline[handler_index](handler_allocator, &handler_arg);
      if (error && config.error_handler)
      {
        ozoneLogWarn("Socket handler_pipeline[%ld] returned %d; will invoke error_handler", handler_index, error);
        config.error_handler(handler_allocator, &handler_arg, error);
        break;
      }
      else if (error)
      {
        ozoneLogError("Socket handler_pipeline[%ld] returned %d; no error_handler is defined so no response will be returned",
                      handler_index, error);
        break;
      }
    }

    if (!skip_write && write(
                           accepted_socket_fd,
                           ((OzoneSocketResponseT *)handler_arg.response)->data,
                           ((OzoneSocketResponseT *)handler_arg.response)->length) < 0)
      ozoneLogError("Could not write (part of) response of length %ld to socket", ((OzoneSocketResponseT *)handler_arg.response)->length);

    close(accepted_socket_fd);
  }

  ozoneAllocatorDelete(handler_allocator);

  return 0;
}
