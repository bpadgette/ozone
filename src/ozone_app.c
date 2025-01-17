#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringMapInsert(event->allocator, &event->response->headers, name, value);
}

void ozoneAppRenderResponseBody(
    OzoneAppEvent* event,
    const OzoneAppContext* context,
    const OzoneString* content_type,
    const OzoneString* component_name,
    const OzoneStringMap* arguments) {
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), content_type);

  OzoneTemplatesComponent* component;
  ozoneVectorForEach(component, &context->templates.components) {
    if (!ozoneStringCompare(component_name, &component->name)) {
      event->response->body = *ozoneTemplatesComponentRender(event->allocator, component, arguments);
      return;
    }
  }
}

int ozoneAppBeginPipeline(OzoneHTTPEvent* event, OzoneAppContext* context) {
  ozoneLogTrace(
      "Beginning app pipeline, %ld hot bytes in event allocator", ozoneAllocatorGetTotalFree(event->allocator));
  context->router.handler_context = context;

  return ozoneRouter(event, &context->router);
}

int ozoneAppServe(unsigned short int port, OzoneAppEndpointVector* endpoints, OzoneStringVector* template_paths) {
  ozoneLogInfo(
      "Registering %ld routes and %ld templates", ozoneVectorLength(endpoints), ozoneVectorLength(template_paths));

  OzoneAllocator* allocator = ozoneAllocatorCreate(4096);

  OzoneTemplatesConfig templates_config = (OzoneTemplatesConfig) { 0 };

  OzoneString* component_path;
  ozoneVectorForEach(component_path, template_paths) {
    OzoneTemplatesComponent* component = ozoneTemplatesComponentFromFile(allocator, component_path);
    ozoneVectorPushOzoneTemplatesComponent(allocator, &templates_config.components, component);
  }

  OzoneAppContext context = (OzoneAppContext) {
    .router = ((OzoneRouterConfig) {
        .endpoints = (*(OzoneRouterHTTPEndpointVector*)endpoints),
        .handler_context = NULL,
    }),
    .templates = templates_config,
  };

  OzoneHTTPConfig http_config = (OzoneHTTPConfig) {
    .port = port,
    .handler_pipeline = ozoneVectorFromElements(OzoneSocketHandlerRef, (OzoneSocketHandlerRef)ozoneAppBeginPipeline),
    .handler_context = &context,
  };

  int return_code = ozoneHTTPServe(allocator, &http_config);
  ozoneAllocatorDelete(allocator);
  return return_code;
}
