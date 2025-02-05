#include "ozone_app.h"

#include "ozone_file.h"
#include "ozone_log.h"
#include <pthread.h>

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneStringMapInsert(event->allocator, &event->response->headers, name, value);
}

void ozoneAppRenderOzoneShellHTML(OzoneAppEvent* event, const OzoneString* title) {
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/html"));

  OzoneString* shell_template_path = ozoneStringCopy(
      event->allocator,
      ozoneStringMapFindValue(
          &event->context->startup_configuration, &ozoneStringConstant(OZONE_APP_OPTION_TEMPLATES_BASE_PATH_KEY)));

  ozoneStringConcatenate(event->allocator, shell_template_path, &ozoneStringConstant("/ozone_shell.html"));

  OzoneString* shell_template_content = ozoneCacheGet(event->allocator, event->context->cache, shell_template_path);
  if (!shell_template_content) {
    OzoneStringVector load_file = (OzoneStringVector) { 0 };
    ozoneFileLoadFromPath(event->allocator, &load_file, shell_template_path, 1024);
    shell_template_content = ozoneStringJoin(event->allocator, &load_file);

    ozoneCacheSet(event->context->allocator, event->context->cache, shell_template_path, shell_template_content);
  }

  OzoneTemplatesComponent* component = ozoneTemplatesComponentCreate(
      event->allocator, shell_template_path, &ozoneVectorFromElements(OzoneString, *shell_template_content));
  OzoneStringMap template_arguments = (OzoneStringMap) { 0 };
  ozoneStringMapInsert(event->allocator, &template_arguments, &ozoneStringConstant("title"), title);
  ozoneStringMapInsert(event->allocator, &template_arguments, &ozoneStringConstant("body"), &event->response->body);

  event->response->body = *ozoneTemplatesComponentRender(event->allocator, component, &template_arguments);
}

int ozoneAppBeginPipeline(OzoneAppEvent* event) {
  ozoneLogTrace(
      "Beginning app pipeline, %ld live bytes in event allocator", ozoneAllocatorGetTotalFree(event->allocator));

  OzoneAppEndpoint* endpoint;
  ozoneVectorForEach(endpoint, &event->context->endpoints) {
    if (event->request->method != endpoint->method)
      continue;

    if (ozoneStringCompare(&event->request->target, &endpoint->target_pattern) != 0)
      continue;

    OzoneSocketHandlerRef* handler;
    ozoneVectorForEach(handler, &endpoint->handler_pipeline) { (*handler)((OzoneSocketEvent*)event); }
    return 0;
  }

  event->response->code = 404;
  return 0;
}

int ozoneAppServe(unsigned short int port, OzoneAppEndpointVector* endpoints, OzoneStringVector* options) {
  OzoneCache cache = (OzoneCache) { 0 };
  pthread_mutex_init(&cache.thread_lock, NULL);

  OzoneAppContext context
      = (OzoneAppContext) { .allocator = ozoneAllocatorCreate(4096), .endpoints = *endpoints, .cache = &cache };

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

  OzoneHTTPConfig http_config = (OzoneHTTPConfig) {
    .port = port,
    .handler_pipeline = ozoneVectorFromElements(OzoneSocketHandlerRef, (OzoneSocketHandlerRef)ozoneAppBeginPipeline),
    .handler_context = &context
  };

  int return_code = ozoneHTTPServe(&http_config);

  pthread_mutex_destroy(&cache.thread_lock);
  ozoneAllocatorDelete(context.allocator);

  return return_code;
}
