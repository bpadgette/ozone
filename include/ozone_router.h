#ifndef OZONE_ROUTER_H
#define OZONE_ROUTER_H

#include "ozone_http.h"
#include "ozone_string.h"

typedef struct OzoneRouterHTTPConfigStruct {
  OzoneHTTPMethod method;
  OzoneString target_pattern;
} OzoneRouterHTTPConfig;

typedef struct OzoneRouterHTTPEndpointStruct {
  OzoneRouterHTTPConfig config;
  OzoneSocketHandlerRefVector handler_pipeline;
} OzoneRouterHTTPEndpoint;

OZONE_VECTOR_DECLARE_API(OzoneRouterHTTPEndpoint)

typedef struct OzoneRouterConfigStruct {
  OzoneRouterHTTPEndpointVector endpoints;
} OzoneRouterConfig;

int ozoneRouter(OzoneRouterConfig* config, OzoneHTTPEvent* event, const void* context);

#endif
