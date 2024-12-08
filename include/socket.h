#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "allocator.h"
#include "array.h"

#define OZ_SOCKET_REQUEST_MAX_LENGTH 16 * 1024
#define OZ_SOCKET_REQUEST_CHUNK_LENGTH 1024

#define OZ_SOCKET_INITIAL_ALLOCATION (OZ_SOCKET_REQUEST_MAX_LENGTH)

typedef OZArrayStringT OZSocketRequestT;
#define ozSocketRequestCreate(alloc, size) (OZSocketRequestT *)ozArrayStringCreate(alloc, size)

typedef OZArrayStringT OZSocketResponseT;
#define ozSocketResponseCreate(alloc, size) (OZSocketResponseT *)ozArrayStringCreate(alloc, size)

typedef struct OZSocketHandlerParameter
{
  void *request;
  void *response;
} OZSocketHandlerParameterT;

typedef int(OZSocketHandlerT)(OZAllocatorT *alloc, OZSocketHandlerParameterT *param);
typedef void(OZSocketErrorHandlerT)(OZAllocatorT *alloc, OZSocketHandlerParameterT *param, int error);

typedef struct OZSocketConfig
{
  unsigned short int port;
  OZSocketHandlerT **handler_pipeline;
  size_t handler_pipeline_length;
  OZSocketErrorHandlerT *error_handler;
} OZSocketConfigT;

int ozSocketServeTCP(OZSocketConfigT config);

#endif
