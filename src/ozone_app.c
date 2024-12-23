#include "ozone_app.h"

#include "ozone_log.h"

int ozoneAppRouter(OzoneAppContextT* context) {
  size_t route_index = 0;
  for (; route_index < context->application->route_count; route_index++) {
    if (context->parsed_request->method != context->application->route_configs[route_index].method)
      continue;

    if (ozoneStringCompare(
            &context->parsed_request->target, &context->application->route_configs[route_index].target_pattern)
        != 0)
      continue;

    break;
  }

  if (route_index >= context->application->route_count) {
    context->parsed_response->code = 404;
    context->parsed_response->body = ozoneHTTPStatusString(404);
    return 0;
  }

  OzoneAppHandlerT** handler_pipeline = (OzoneAppHandlerT**)context->application->route_handler_pipelines[route_index];
  size_t handler_pipeline_count = context->application->route_handler_pipelines_counts[route_index];
  for (size_t handler_index = 0; handler_index < handler_pipeline_count; handler_index++) {
    handler_pipeline[handler_index](context);
  }

  return 0;
}

int ozoneAppServe(OzoneAllocatorT* allocator, OzoneAppConfigT config) {
  OzoneHTTPConfigT http_config = { 0 };
  http_config.port = config.port;

  ozoneLogInfo("Registering %ld routes", config.routes_count);

  OzoneAppApplicationContextT* app = ozoneAllocatorReserveOne(allocator, OzoneAppApplicationContextT);

  app->route_configs = ozoneAllocatorReserveMany(allocator, OzoneAppHTTPRouteConfigT, config.routes_count);
  app->route_handler_pipelines = ozoneAllocatorReserveMany(allocator, OzoneHTTPHandlerT**, config.routes_count);
  app->route_handler_pipelines_counts = ozoneAllocatorReserveMany(allocator, size_t, config.routes_count);

  app->route_count = config.routes_count;
  for (size_t route_index = 0; route_index < config.routes_count; route_index++) {
    app->route_configs[route_index] = config.routes[route_index].config;
    app->route_handler_pipelines[route_index] = (OzoneHTTPHandlerT**)config.routes[route_index].handler_pipeline;
    app->route_handler_pipelines_counts[route_index] = config.routes[route_index].handler_pipeline_count;
  }

  http_config.application = (void*)app;

  OzoneHTTPHandlerT* handler_pipeline[] = { (OzoneHTTPHandlerT*)ozoneAppRouter };
  http_config.handler_pipeline = handler_pipeline;
  http_config.handler_pipeline_count = 1;

  return ozoneHTTPServe(allocator, http_config);
}
