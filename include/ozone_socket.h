#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "ozone_allocator.h"

typedef struct OzoneSocketChunk {
  char* buffer;
  size_t length;
  struct OzoneSocketChunk* next;
} OzoneSocketChunkT;

#define OZONE_SOCKET_CONTEXT_FIELDS(_parsed_request_type_, _parsed_response_type_, _app_context_type_)                 \
  {                                                                                                                    \
    OzoneAllocatorT* allocator;                                                                                        \
    const OzoneSocketChunkT* raw_request;                                                                              \
    OzoneSocketChunkT* raw_response;                                                                                   \
    _parsed_request_type_* parsed_request;                                                                             \
    _parsed_response_type_* parsed_response;                                                                           \
    _app_context_type_* application;                                                                                   \
  }

typedef struct OzoneSocketContext OZONE_SOCKET_CONTEXT_FIELDS(void, void, void) OzoneSocketContextT;

typedef int(OzoneSocketHandlerT)(OzoneSocketContextT* context);

typedef struct OzoneSocketConfig {
  unsigned short int port;
  OzoneSocketHandlerT** handler_pipeline;
  size_t handler_pipeline_count;
  void* application;
} OzoneSocketConfigT;

int ozoneSocketServeTCP(OzoneSocketConfigT config);

#endif
