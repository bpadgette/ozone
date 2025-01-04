#ifndef OZONE_ROUTER_H
#define OZONE_ROUTER_H

#include "ozone_http.h"
#include "ozone_string.h"

typedef struct OzoneRouterHTTPConfigStruct {
  OzoneHTTPMethod method;
  OzoneString target_pattern;
} OzoneRouterHTTPConfig;

typedef struct OzoneRouterApplicationContextStruct {
  OzoneRouterHTTPConfig* route_configs;
  OzoneHTTPHandler*** route_handler_pipelines;
  size_t* route_handler_pipelines_counts;
  size_t route_count;
} OzoneRouterApplicationContext;

typedef struct OzoneRouterContextStruct OZONE_SOCKET_CONTEXT_FIELDS(
    OzoneHTTPRequest, OzoneHTTPResponse, OzoneRouterApplicationContext) OzoneRouterContext;

int ozoneRouter(OzoneRouterContext* context);

typedef struct OzoneRouterHTTPEndpointStruct {
  OzoneRouterHTTPConfig config;
  OzoneHTTPHandler** handler_pipeline;
  size_t handler_pipeline_count;
} OzoneRouterHTTPEndpoint;

typedef struct OzoneRouterConfigStruct {
  OzoneRouterHTTPEndpoint* endpoints;
  size_t endpoints_count;
} OzoneRouterConfig;

#define ozoneRouterConfig(_endpoints_)                                                                                 \
  (OzoneRouterConfig) {                                                                                                \
    .endpoints = _endpoints_, .endpoints_count = sizeof(_endpoints_) / sizeof(OzoneRouterHTTPEndpoint)                 \
  }

#endif
