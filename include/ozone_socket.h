#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "ozone_allocator.h"

typedef struct OzoneSocketChunkStruct {
  char* buffer;
  size_t length;
  struct OzoneSocketChunkStruct* next;
} OzoneSocketChunk;

#define OZONE_SOCKET_CONTEXT_FIELDS(_parsed_request_type_, _parsed_response_type_, _app_context_type_)                 \
  {                                                                                                                    \
    OzoneAllocator* allocator;                                                                                         \
    const OzoneSocketChunk* raw_request;                                                                               \
    OzoneSocketChunk* raw_response;                                                                                    \
    _parsed_request_type_* parsed_request;                                                                             \
    _parsed_response_type_* parsed_response;                                                                           \
    _app_context_type_* application;                                                                                   \
  }

typedef struct OzoneSocketContextStruct OZONE_SOCKET_CONTEXT_FIELDS(void, void, void) OzoneSocketContext;

typedef int(OzoneSocketHandler)(OzoneSocketContext* context);

typedef struct OzoneSocketConfigStruct {
  unsigned short int port;
  OzoneSocketHandler** handler_pipeline;
  size_t handler_pipeline_count;
  void* application;
} OzoneSocketConfig;

int ozoneSocketServeTCP(OzoneSocketConfig config);

#endif
