#include <stdio.h>
#include <errno.h>
#include "allocator.h"
#include "socket.h"
#include "log.h"

OZSocketResponseT *handler(OZAllocatorT *alloc, OZSocketRequestT *request)
{
  ozLogDebug("Request Size: %ld", request->size);

  OZSocketResponseT *response = ozSocketResponseCreate(alloc, 70);
  snprintf(response->data,
           response->size,
           "HTTP/1.0 200 OK\r\n"
           "Content-length: 3\r\n"
           "Content-type: text/plain\r\n\r\n"
           "ooo\r\n");

  return response;
}

int main(int argc, char **argv)
{
  ozLogInfo("Launching");
  return ozServeSocket(handler);
}
