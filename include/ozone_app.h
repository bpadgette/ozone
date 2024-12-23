#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_http.h"
#include "ozone_socket.h"
#include "ozone_string.h"

typedef struct OzoneAppHTTPRouteConfig {
  OzoneHTTPMethodT method;
  OzoneStringT target_pattern;
} OzoneAppHTTPRouteConfigT;

typedef struct OzoneAppApplicationContext {
  OzoneAppHTTPRouteConfigT* route_configs;
  OzoneHTTPHandlerT*** route_handler_pipelines;
  size_t* route_handler_pipelines_counts;
  size_t route_count;
} OzoneAppApplicationContextT;

typedef struct OzoneAppContext OZONE_SOCKET_CONTEXT_FIELDS(
    OzoneHTTPRequestT, OzoneHTTPResponseT, OzoneAppApplicationContextT) OzoneAppContextT;

typedef int(OzoneAppHandlerT)(OzoneAppContextT* context);

typedef struct OzoneAppRoute {
  OzoneAppHTTPRouteConfigT config;
  OzoneAppHandlerT** handler_pipeline;
  size_t handler_pipeline_count;
} OzoneAppRouteT;

#define ozoneAppRoute(_method_, _target_pattern_, _handler_pipeline_)                                                  \
  (OzoneAppRouteT) {                                                                                                   \
    .config = (OzoneAppHTTPRouteConfigT) { .method = OZONE_HTTP_METHOD_##_method_,                                     \
      .target_pattern = ozoneCharArray(_target_pattern_) },                                                            \
    .handler_pipeline = _handler_pipeline_,                                                                            \
    .handler_pipeline_count = sizeof(_handler_pipeline_) / sizeof(OzoneAppHandlerT*)                                   \
  }

typedef struct OzoneAppConfig {
  unsigned short int port;
  OzoneAppRouteT* routes;
  size_t routes_count;
} OzoneAppConfigT;

int ozoneAppServe(OzoneAllocatorT* allocator, OzoneAppConfigT config);

#endif
