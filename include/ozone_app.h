#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_http.h"
#include "ozone_router.h"
#include "ozone_socket.h"
#include "ozone_string.h"
#include "ozone_templates.h"

typedef OzoneHTTPEvent OzoneAppEvent;
typedef OzoneRouterHTTPEndpoint OzoneAppEndpoint;
OZONE_VECTOR_DECLARE_API(OzoneAppEndpoint)

#define ozoneAppEndpoint(_method_, _path_, _handler_pipeline_)                                                         \
  (OzoneAppEndpoint) {                                                                                                 \
    .config = ((OzoneRouterHTTPConfig) {                                                                               \
        .method = OZONE_HTTP_METHOD_##_method_,                                                                        \
        .target_pattern = ozoneStringConstant(_path_),                                                                 \
    }),                                                                                                                \
    .handler_pipeline = ozoneVectorFromArray(OzoneSocketHandlerRef, _handler_pipeline_),                               \
  }

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value);

typedef struct OzoneAppContextStruct {
  OzoneRouterConfig router;
  OzoneTemplatesConfig templates;
} OzoneAppContext;

typedef int(OzoneAppHandler)(OzoneAppEvent* event, OzoneAppContext* context);

int ozoneAppServe(
    OzoneAllocator* allocator,
    unsigned short int port,
    OzoneAppEndpointVector* endpoints,
    OzoneTemplatesComponentVector* templates);

#endif
