#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "ozone_allocator.h"
#include "ozone_array.h"

#define OZONE_SOCKET_REQUEST_MAX_LENGTH 16 * 1024
#define OZONE_SOCKET_REQUEST_CHUNK_LENGTH 1024

#define OZONE_SOCKET_INITIAL_ALLOCATION (OZONE_SOCKET_REQUEST_MAX_LENGTH)

typedef struct OzoneSocketHandlerContext
{
  OzoneAllocatorT *allocator;
  void *request;
  void *response;
} OzoneSocketHandlerContextT;

typedef int(OzoneSocketHandlerT)(OzoneSocketHandlerContextT *context);
typedef void(OzoneSocketErrorHandlerT)(OzoneSocketHandlerContextT *context, int error);

typedef struct OzoneSocketConfig
{
  unsigned short int port;
  OzoneSocketHandlerT **handler_pipeline;
  size_t handler_pipeline_length;
  OzoneSocketErrorHandlerT *error_handler;
} OzoneSocketConfigT;

int ozoneSocketServeTCP(OzoneSocketConfigT config);

#endif
