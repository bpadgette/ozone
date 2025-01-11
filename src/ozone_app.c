#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringPushKeyValue(event->allocator, &event->response->headers, name, value);
}

int ozoneAppHandler(OzoneHTTPEvent* event, OzoneAppContext* context) {
  clearOzoneStringKeyValue(&context->templates.arguments);
  context->router.handler_context = context;
  return ozoneRouter(event, &context->router);
}

int ozoneAppServe(
    OzoneAllocator* allocator,
    unsigned short int port,
    const OzoneAppEndpointVector* endpoints,
    const OzoneTemplatesComponentVector* templates) {
  ozoneLogInfo("Registering %ld routes", ozoneVectorLength(endpoints));

  OzoneSocketHandlerRef app_stack[] = {
    (OzoneSocketHandlerRef)ozoneAppHandler,
  };

  OzoneAppContext context = (OzoneAppContext) {
      .router = (OzoneRouterConfig) {
          .endpoints = (const OzoneRouterHTTPEndpointVector*)endpoints,
          .handler_context = NULL,
      },
      .templates = (OzoneTemplatesConfig) {
        .arguments = ((OzoneStringKeyValueVector) { 0 }),
        .components = *templates
      },
  };

  OzoneHTTPConfig http_config = {
    .port = port,
    .handler_pipeline = ozoneVector(OzoneSocketHandlerRef, app_stack),
    .handler_context = &context,
  };

  return ozoneHTTPServe(allocator, http_config);
}
