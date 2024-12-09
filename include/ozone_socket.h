#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "ozone_allocator.h"
#include "ozone_array.h"

#define OZONE_SOCKET_REQUEST_MAX_LENGTH 16 * 1024
#define OZONE_SOCKET_REQUEST_CHUNK_LENGTH 1024

#define OZONE_SOCKET_INITIAL_ALLOCATION (OZONE_SOCKET_REQUEST_MAX_LENGTH)

typedef OzoneArrayStringT OzoneSocketRequestT;
#define ozoneSocketRequestCreate(alloc, size) (OzoneSocketRequestT *)ozoneArrayStringCreate(alloc, size)

typedef OzoneArrayStringT OzoneSocketResponseT;
#define ozoneSocketResponseCreate(alloc, size) (OzoneSocketResponseT *)ozoneArrayStringCreate(alloc, size)

typedef struct OzoneSocketHandlerParameter
{
  void *request;
  void *response;
} OzoneSocketHandlerParameterT;

typedef int(OzoneSocketHandlerT)(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param);
typedef void(OzoneSocketErrorHandlerT)(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param, int error);

typedef struct OzoneSocketConfig
{
  unsigned short int port;
  OzoneSocketHandlerT **handler_pipeline;
  size_t handler_pipeline_length;
  OzoneSocketErrorHandlerT *error_handler;
} OzoneSocketConfigT;

int ozoneSocketServeTCP(OzoneSocketConfigT config);

#endif
