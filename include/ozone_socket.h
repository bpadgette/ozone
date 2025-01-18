#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H

#include "ozone_allocator.h"
#include "ozone_string.h"
#include "ozone_vector.h"

#define OZONE_SOCKET_EVENT_FIELDS(_request_type_, _response_type_)                                                     \
  {                                                                                                                    \
    OzoneAllocator* allocator;                                                                                         \
    OzoneStringVector raw_socket_request;                                                                              \
    OzoneStringVector raw_socket_response;                                                                             \
    _request_type_* request;                                                                                           \
    _response_type_* response;                                                                                         \
  }

typedef struct OzoneSocketEventStruct OZONE_SOCKET_EVENT_FIELDS(void, void) OzoneSocketEvent;

typedef int(OzoneSocketHandler)(OzoneSocketEvent* event, const void* context);
typedef OzoneSocketHandler* OzoneSocketHandlerRef;
OZONE_VECTOR_DECLARE_API(OzoneSocketHandlerRef)

typedef struct OzoneSocketConfigStruct {
  unsigned short int port;
  OzoneSocketHandlerRefVector handler_pipeline;
} OzoneSocketConfig;

int ozoneSocketServeTCP(OzoneSocketConfig* config, const void* context);

#endif
