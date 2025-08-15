#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H

#include "ozone_allocator.h"
#include "ozone_map.h"
#include "ozone_string.h"
#include "ozone_vector.h"

#define OZONE_SOCKET_EVENT_FIELDS(_request_type_, _response_type_, _context_type_)                                     \
  {                                                                                                                    \
    OzoneAllocator* allocator;                                                                                         \
    OzoneStringMap parameters;                                                                                         \
    OzoneStringVector raw_socket_request;                                                                              \
    OzoneStringVector raw_socket_response;                                                                             \
    _request_type_* request;                                                                                           \
    _response_type_* response;                                                                                         \
    const _context_type_* context;                                                                                     \
  }

typedef struct OzoneSocketEventStruct OZONE_SOCKET_EVENT_FIELDS(void, void, void) OzoneSocketEvent;

typedef void(OzoneSocketHandler)(OzoneSocketEvent* event);
typedef OzoneSocketHandler* OzoneSocketHandlerRef;
OZONE_VECTOR_DECLARE_API(OzoneSocketHandlerRef)

typedef struct OzoneSocketConfigStruct {
  void* handler_context;
  OzoneSocketHandlerRefVector handler_pipeline;
  unsigned int workers;
  unsigned int port;
} OzoneSocketConfig;

int ozoneSocketServeTCP(OzoneSocketConfig* config);

#endif
