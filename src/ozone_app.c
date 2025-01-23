#include "ozone_app.h"

#include "ozone_file.h"
#include "ozone_log.h"
#include "ozone_router.h"

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringMapInsert(event->allocator, &event->response->headers, name, value);
}

OzoneTemplatesComponent*
ozoneAppBootstrapTemplateComponent(OzoneAppContext* context, const OzoneString* component_path) {
  OzoneTemplatesComponent* component;
  ozoneVectorForEach(component, &context->templates.components) {
    if (!ozoneStringCompare(component_path, &component->name)) {
      return component;
    }
  }

  ozoneLogInfo("Will create template component from file %s", ozoneStringBuffer(component_path));
  ozoneVectorPushOzoneTemplatesComponent(
      context->allocator,
      &context->templates.components,
      ozoneTemplatesComponentFromFile(context->allocator, component_path));

  return ozoneVectorLast(&context->templates.components);
}

void ozoneAppRenderOzoneShellHTML(
    OzoneAppEvent* event, OzoneAppContext* context, const OzoneString* title, const OzoneString* body) {
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/html"));

  OzoneString* templates_base_path = ozoneStringCopy(
      event->allocator,
      ozoneStringMapFindValue(
          &context->startup_configuration, &ozoneStringConstant(OZONE_APP_OPTION_TEMPLATES_BASE_PATH_KEY)));

  ozoneStringConcatenate(event->allocator, templates_base_path, &ozoneStringConstant("/ozone_shell.html"));

  OzoneTemplatesComponent* component = ozoneAppBootstrapTemplateComponent(context, templates_base_path);

  OzoneStringMap template_arguments = (OzoneStringMap) { 0 };
  ozoneStringMapInsert(event->allocator, &template_arguments, &ozoneStringConstant("title"), title);
  ozoneStringMapInsert(event->allocator, &template_arguments, &ozoneStringConstant("body"), body);

  event->response->body = *ozoneTemplatesComponentRender(event->allocator, component, &template_arguments);
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
    .allocator = ozoneAllocatorCreate(4096),
    .router = ((OzoneRouterConfig) {
        .endpoints = *((OzoneRouterHTTPEndpointVector*)endpoints),
    }),
  };

  OzoneString* ozone_templates_base_path = ozoneString(context.allocator, "/usr/include/ozone/html");

  if (options) {
    OzoneString* option_key_value;
    ozoneVectorForEach(option_key_value, options) {
      int equals_at = ozoneStringFindFirst(option_key_value, &ozoneStringConstant("="));
      if (equals_at < 0) {
        continue;
      }

      OzoneString* key = ozoneStringSlice(context.allocator, option_key_value, 0, equals_at);
      OzoneString* value
          = ozoneStringSlice(context.allocator, option_key_value, equals_at + 1, ozoneStringLength(option_key_value));

      if (!ozoneStringCompare(key, &ozoneStringConstant(OZONE_APP_OPTION_TEMPLATES_BASE_PATH_KEY))) {
        ozone_templates_base_path = value;
      } else {
        ozoneLogWarn("Ignored option %s", ozoneStringBuffer(key));
      }
    }
  }

  ozoneStringMapInsert(
      context.allocator,
      &context.startup_configuration,
      &ozoneStringConstant(OZONE_APP_OPTION_TEMPLATES_BASE_PATH_KEY),
      ozone_templates_base_path);

  int return_code = ozoneHTTPServe(&http_config, &context);
  ozoneAllocatorDelete(context.allocator);

  return return_code;
}
