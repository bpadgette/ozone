#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_http.h"
#include "ozone_router.h"
#include "ozone_socket.h"
#include "ozone_string.h"
#include "ozone_templates.h"

#define OZONE_APP_OPTION_TEMPLATES_BASE_PATH_KEY "ozone-templates-base-path"
#define OZONE_APP_OPTION_OZONE_JS_KEY "ozone-js"

typedef OzoneHTTPEvent OzoneAppEvent;
typedef OzoneRouterHTTPEndpoint OzoneAppEndpoint;
OZONE_VECTOR_DECLARE_API(OzoneAppEndpoint)

#define ozoneAppEndpoint(_method_, _path_, ...)                                                                        \
  (OzoneAppEndpoint) {                                                                                                 \
    .config = ((OzoneRouterHTTPConfig) {                                                                               \
        .method = OZONE_HTTP_METHOD_##_method_,                                                                        \
        .target_pattern = ozoneStringConstant(_path_),                                                                 \
    }),                                                                                                                \
    .handler_pipeline = ozoneVectorFromArray(OzoneSocketHandlerRef, ((OzoneAppHandler*[]) { __VA_ARGS__ }))            \
  }

typedef struct OzoneAppContextStruct {
  OzoneAllocator* allocator;
  OzoneRouterConfig router;
  OzoneTemplatesConfig templates;
  OzoneStringMap startup_configuration;
} OzoneAppContext;

typedef int(OzoneAppHandler)(OzoneAppEvent* event, OzoneAppContext* context);

int ozoneAppServe(unsigned short int port, OzoneAppEndpointVector* endpoints, OzoneStringVector* options);

void ozoneAppRenderOzoneShellHTML(
    OzoneAppEvent* event, OzoneAppContext* context, const OzoneString* title, const OzoneString* body);

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value);

#endif
