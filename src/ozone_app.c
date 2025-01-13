#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringMapInsert(event->allocator, &event->response->headers, name, value);
}

int ozoneAppBeginPipeline(OzoneHTTPEvent* event, OzoneAppContext* context) {
  context->templates->arguments = NULL;
  context->router->handler_context = context;

  return ozoneRouter(event, context->router);
}

int ozoneAppServe(
    OzoneAllocator* allocator,
    unsigned short int port,
    OzoneAppEndpointVector* endpoints,
    OzoneTemplatesComponentVector* templates) {
  ozoneLogInfo("Registering %ld routes", ozoneVectorLength(endpoints));

  OzoneRouterConfig* router_config = ozoneAllocatorReserveOne(allocator, OzoneRouterConfig);
  router_config->endpoints = (const OzoneRouterHTTPEndpointVector*)endpoints;

  OzoneTemplatesConfig* templates_config = ozoneAllocatorReserveOne(allocator, OzoneTemplatesConfig);
  templates_config->components = templates;

  OzoneAppContext* context = ozoneAllocatorReserveOne(allocator, OzoneAppContext);
  context->router = router_config;
  context->templates = templates_config;

  OzoneSocketHandlerRef app_stack[] = {
    (OzoneSocketHandlerRef)ozoneAppBeginPipeline,
  };

  OzoneHTTPConfig http_config = {
    .port = port,
    .handler_pipeline = &ozoneVectorFromArray(OzoneSocketHandlerRef, app_stack),
    .handler_context = context,
  };

  return ozoneHTTPServe(allocator, &http_config);
}
