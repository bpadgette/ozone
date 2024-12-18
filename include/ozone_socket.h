#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "ozone_allocator.h"

typedef struct OzoneSocketChunk {
  char* buffer;
  size_t length;
  struct OzoneSocketChunk* next;
} OzoneSocketChunkT;

#define OZONE_SOCKET_HANDLER_CONTEXT_FIELDS(_application_type_, _parsed_type_)                                         \
  {                                                                                                                    \
    OzoneAllocatorT* allocator;                                                                                        \
    const OzoneSocketChunkT* request;                                                                                  \
    OzoneSocketChunkT* response;                                                                                       \
    _application_type_* application;                                                                                   \
    _parsed_type_* parsed;                                                                                             \
  }

typedef struct OzoneSocketHandlerContext OZONE_SOCKET_HANDLER_CONTEXT_FIELDS(void, void) OzoneSocketHandlerContextT;

typedef int(OzoneSocketHandlerT)(OzoneSocketHandlerContextT* context);

typedef struct OzoneSocketConfig {
  unsigned short int port;
  OzoneSocketHandlerT** handler_pipeline;
  size_t handler_pipeline_count;
  void* application;
} OzoneSocketConfigT;

int ozoneSocketServeTCP(OzoneSocketConfigT config);

#endif
