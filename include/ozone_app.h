#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_http.h"
#include "ozone_router.h"
#include "ozone_socket.h"
#include "ozone_string.h"
#include "ozone_templates.h"

typedef OzoneRouterHTTPEndpoint OzoneAppEndpoint;

#define ozoneAppEndpoint(_method_, _target_pattern_, _handler_pipeline_)                                               \
  ((OzoneAppEndpoint) {                                                                                                \
      .config = ((OzoneRouterHTTPConfig) {                                                                             \
          .method = OZONE_HTTP_METHOD_##_method_,                                                                      \
          .target_pattern = ozoneString(_target_pattern_),                                                             \
      }),                                                                                                              \
      .handler_pipeline = ozoneVector(OzoneSocketHandlerRef, _handler_pipeline_),                                      \
  })

OZONE_VECTOR_DECLARE_API(OzoneAppEndpoint)

typedef OzoneHTTPEvent OzoneAppEvent;

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value);

typedef struct OzoneAppContextStruct {
  OzoneRouterConfig router;
  OzoneTemplatesConfig templates;
} OzoneAppContext;

typedef int(OzoneAppHandler)(OzoneAppEvent* event, OzoneAppContext* context);
typedef OzoneAppHandler* OzoneAppHandlerRef;

int ozoneAppServe(
    OzoneAllocator* allocator,
    unsigned short int port,
    const OzoneAppEndpointVector* endpoints,
    const OzoneTemplatesComponentVector* templates);

#endif
