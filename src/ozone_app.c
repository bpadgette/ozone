#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringMapInsert(event->allocator, &event->response->headers, name, value);
}

void ozoneAppRenderResponseBody(
    OzoneAppEvent* event,
    OzoneAppContext* context,
    const OzoneString* content_type,
    const OzoneString* component_path,
    const OzoneStringMap* arguments) {
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), content_type);

  OzoneTemplatesComponent* component;
  ozoneVectorForEach(component, &context->templates.components) {
    if (!ozoneStringCompare(component_path, &component->name)) {
      event->response->body = *ozoneTemplatesComponentRender(event->allocator, component, arguments);
      return;
    }
  }

  ozoneVectorPushOzoneTemplatesComponent(
      event->allocator,
      &context->templates.components,
      ozoneTemplatesComponentFromFile(event->allocator, component_path));

  event->response->body
      = *ozoneTemplatesComponentRender(event->allocator, ozoneVectorLast(&context->templates.components), arguments);
}

int ozoneAppBeginPipeline(OzoneHTTPEvent* event, OzoneAppContext* context) {
  ozoneLogTrace(
      "Beginning app pipeline, %ld live bytes in event allocator", ozoneAllocatorGetTotalFree(event->allocator));

  return ozoneRouter(&context->router, event, context);
}

int ozoneAppServe(unsigned short int port, OzoneAppEndpointVector* endpoints, OzoneStringVector* options) {
  OzoneHTTPConfig http_config = (OzoneHTTPConfig) {
    .port = port,
    .handler_pipeline = ozoneVectorFromElements(OzoneSocketHandlerRef, (OzoneSocketHandlerRef)ozoneAppBeginPipeline),
  };

  OzoneAppContext context = (OzoneAppContext) {
    .router = ((OzoneRouterConfig) {
        .endpoints = *((OzoneRouterHTTPEndpointVector*)endpoints),
    }),
  };

  OzoneAllocator* allocator = ozoneAllocatorCreate(4096);
  if (options) {
    OzoneString* option_key_value;
    ozoneVectorForEach(option_key_value, options) {
      int equals_at = ozoneStringFindFirst(option_key_value, &ozoneStringConstant("="));
      if (equals_at < 0) {
        continue;
      }

      OzoneString* key = ozoneStringSlice(allocator, option_key_value, 0, equals_at);
      OzoneString* value
          = ozoneStringSlice(allocator, option_key_value, equals_at + 1, ozoneStringLength(option_key_value));

      if (!ozoneStringCompare(key, &ozoneStringConstant("template"))) {
        ozoneVectorPushOzoneTemplatesComponent(
            allocator, &context.templates.components, ozoneTemplatesComponentFromFile(allocator, value));
      } else {
        ozoneLogWarn("Ignored option %s", ozoneStringBuffer(key));
      }
    }
  }

  int return_code = ozoneHTTPServe(allocator, &http_config, &context);
  ozoneAllocatorDelete(allocator);

  return return_code;
}
