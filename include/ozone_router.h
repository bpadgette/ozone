#ifndef OZONE_ROUTER_H
#define OZONE_ROUTER_H

#include "ozone_http.h"
#include "ozone_string.h"

typedef struct OzoneRouterHTTPConfig {
  OzoneHTTPMethodT method;
  OzoneStringT target_pattern;
} OzoneRouterHTTPConfigT;

typedef struct OzoneRouterApplicationContext {
  OzoneRouterHTTPConfigT* route_configs;
  OzoneHTTPHandlerT*** route_handler_pipelines;
  size_t* route_handler_pipelines_counts;
  size_t route_count;
} OzoneRouterApplicationContextT;

typedef struct OzoneRouterContext OZONE_SOCKET_CONTEXT_FIELDS(
    OzoneHTTPRequestT, OzoneHTTPResponseT, OzoneRouterApplicationContextT) OzoneRouterContextT;

int ozoneRouter(OzoneRouterContextT* context);

typedef struct OzoneRouterHTTPEndpoint {
  OzoneRouterHTTPConfigT config;
  OzoneHTTPHandlerT** handler_pipeline;
  size_t handler_pipeline_count;
} OzoneRouterHTTPEndpointT;

typedef struct OzoneRouterConfig {
  OzoneRouterHTTPEndpointT* endpoints;
  size_t endpoints_count;
} OzoneRouterConfigT;

#define ozoneRouterConfig(_endpoints_)                                                                                 \
  (OzoneRouterConfigT) {                                                                                               \
    .endpoints = _endpoints_, .endpoints_count = sizeof(_endpoints_) / sizeof(OzoneRouterHTTPEndpointT)                \
  }

#endif
