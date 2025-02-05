#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_cache.h"
#include "ozone_http.h"
#include "ozone_socket.h"
#include "ozone_string.h"
#include "ozone_templates.h"

#define OZONE_APP_OPTION_TEMPLATES_BASE_PATH_KEY "ozone-templates-base-path"

typedef struct OzoneAppEndpointStruct {
  OzoneHTTPMethod method;
  OzoneString target_pattern;
  OzoneSocketHandlerRefVector handler_pipeline;
} OzoneAppEndpoint;

OZONE_VECTOR_DECLARE_API(OzoneAppEndpoint)

typedef struct OzoneAppContextStruct {
  OzoneAllocator* allocator;
  OzoneAppEndpointVector endpoints;
  OzoneStringMap startup_configuration;
  OzoneCache* cache;
} OzoneAppContext;

typedef struct OzoneAppEventStruct
    OZONE_SOCKET_EVENT_FIELDS(OzoneHTTPRequest, OzoneHTTPResponse, OzoneAppContext) OzoneAppEvent;

typedef int(OzoneAppHandler)(OzoneAppEvent* event);

#define ozoneAppEndpoint(_method_, _path_, ...)                                                                        \
  (OzoneAppEndpoint) {                                                                                                 \
    .method = OZONE_HTTP_METHOD_##_method_, .target_pattern = ozoneStringConstant(_path_),                             \
    .handler_pipeline = ozoneVectorFromArray(OzoneSocketHandlerRef, ((OzoneAppHandler*[]) { __VA_ARGS__ }))            \
  }

int ozoneAppServe(unsigned short int port, OzoneAppEndpointVector* endpoints, OzoneStringVector* options);

void ozoneAppRenderOzoneShellHTML(OzoneAppEvent* event, const OzoneString* title);
void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value);

#endif
