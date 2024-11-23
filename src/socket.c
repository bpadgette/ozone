#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "allocator.h"
#include "socket.h"
#include "log.h"

#define OZ_SOCKET_REQUEST_CHUNK_LENGTH 1024
#define OZ_SOCKET_REQUEST_MAX_LENGTH 8 * OZ_SOCKET_REQUEST_CHUNK_LENGTH

int ozServeSocket(OZSocketHandlerT *handler)
{
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1)
  {
    ozLogError("Could not get socket file descriptor");
    return EACCES;
  }

  struct sockaddr_in host_addr;
  int host_addrlen = sizeof(host_addr);
  host_addr.sin_family = AF_INET;
  host_addr.sin_port = htons(8080);
  host_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(sockfd, (struct sockaddr *)&host_addr, host_addrlen) != 0 || listen(sockfd, SOMAXCONN) != 0)
  {
    ozLogError("Connection aborted");
    return ECONNABORTED;
  }

  OZAllocatorT *handler_allocator = ozAllocatorCreate(2 * OZ_SOCKET_REQUEST_MAX_LENGTH);
  for (;;)
  {
    int newsockfd = accept(
        sockfd,
        (struct sockaddr *)&host_addr,
        (socklen_t *)&host_addrlen);

    if (newsockfd < 0)
    {
      ozLogError("Could not accept connection");
      continue;
    }

    ozAllocatorClear(handler_allocator);
    OZSocketRequestT *request = ozSocketRequestCreate(handler_allocator, OZ_SOCKET_REQUEST_MAX_LENGTH);

    long int bytes_read = 0;
    long int read_status = 0;
    do
    {
      size_t bytes_readable = (bytes_read + OZ_SOCKET_REQUEST_CHUNK_LENGTH) > OZ_SOCKET_REQUEST_MAX_LENGTH
                                  ? OZ_SOCKET_REQUEST_MAX_LENGTH - bytes_read
                                  : OZ_SOCKET_REQUEST_CHUNK_LENGTH;

      read_status = bytes_readable > 0
                        ? read(newsockfd, bytes_read + request->data, bytes_readable)
                        : -2;

      if (read_status > 0 && read_status < bytes_readable)
        read_status = 0;
      else if (read_status == bytes_readable)
        bytes_read += bytes_readable;
      else if (read_status == -1)
        ozLogError(
            "Could not read (part of) a request; internal error, %ld bytes read",
            bytes_read);
      else if (read_status == -1)
        ozLogError(
            "Could not read (part of) a request; out of memory, %ld bytes read, OZ_SOCKET_REQUEST_MAX_LENGTH %d",
            bytes_read, OZ_SOCKET_REQUEST_MAX_LENGTH);
    } while (read_status > 0);

    OZSocketResponseT *response = handler(handler_allocator, request);
    if (write(newsockfd, response->data, response->size) < 0)
      ozLogError("Could not write (part of) response of length %ld to socket", response->size);

    close(newsockfd);
  }

  ozAllocatorDelete(handler_allocator);

  return 0;
}
