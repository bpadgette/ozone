#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringMapInsert(event->allocator, &event->response->headers, name, value);
}

int ozoneAppBeginPipeline(OzoneHTTPEvent* event, OzoneAppContext* context) {
  ozoneLogTrace(
      "Beginning app pipeline, %ld hot bytes in event allocator", ozoneAllocatorGetTotalFree(event->allocator));
  context->templates.arguments = (OzoneStringMap) { 0 };
  context->router.handler_context = context;

  return ozoneRouter(event, &context->router);
}

int ozoneAppServe(
    OzoneAllocator* allocator,
    unsigned short int port,
    OzoneAppEndpointVector* endpoints,
    OzoneTemplatesComponentVector* templates) {
  ozoneLogInfo("Registering %ld routes", ozoneVectorLength(endpoints));

  OzoneTemplatesConfig templates_config = (OzoneTemplatesConfig) { 0 };
  templates_config.components = *templates;

  OzoneAppContext context = (OzoneAppContext) {
    .router = ((OzoneRouterConfig) {
        .endpoints = (*(OzoneRouterHTTPEndpointVector*)endpoints),
        .handler_context = NULL,
    }),
    .templates = templates_config,
  };

  OzoneSocketHandlerRef app_stack[] = {
    (OzoneSocketHandlerRef)ozoneAppBeginPipeline,
  };

  OzoneHTTPConfig http_config = (OzoneHTTPConfig) {
    .port = port,
    .handler_pipeline = ozoneVectorFromArray(OzoneSocketHandlerRef, app_stack),
    .handler_context = &context,
  };

  return ozoneHTTPServe(allocator, &http_config);
}
