#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "ozone_allocator.h"

typedef struct OzoneSocketChunk {
  char* buffer;
  size_t length;
  struct OzoneSocketChunk* next;
} OzoneSocketChunkT;

typedef struct OzoneSocketHandlerContext {
  OzoneAllocatorT* allocator;
  const OzoneSocketChunkT* request;
  OzoneSocketChunkT* response;
  void* extra_context;
} OzoneSocketHandlerContextT;

typedef int(OzoneSocketHandlerT)(OzoneSocketHandlerContextT* context);
typedef int(OzoneSocketErrorHandlerT)(OzoneSocketHandlerContextT* context, int error);

typedef struct OzoneSocketConfig {
  unsigned short int port;
  OzoneSocketHandlerT** handler_pipeline;
  size_t handler_pipeline_length;
  OzoneSocketErrorHandlerT* error_handler;
} OzoneSocketConfigT;

int ozoneSocketServeTCP(OzoneSocketConfigT config);

#endif
