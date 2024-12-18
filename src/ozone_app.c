#include "ozone_app.h"

#include "ozone_log.h"

int ozoneAppRouter(OzoneAppContextT* context)
{
  for (size_t route_index = 0; route_index < context->application_context->route_count; route_index++) {
    if (ozoneStringCompare(&context->request_context->request.target,
            &context->application_context->route_configs[route_index].target_pattern)
        != 0) {
      continue;
    }

    OzoneAppHandlerT** handler_pipeline
        = (OzoneAppHandlerT**)context->application_context->route_handler_pipelines[route_index];
    size_t handler_pipeline_count = context->application_context->route_handler_pipelines_counts[route_index];
    for (size_t handler_index = 0; handler_index < handler_pipeline_count; handler_index++) {
      handler_pipeline[handler_index](context);
    }

    return 0;
  }

  context->request_context->response.code = 404;
  return 0;
}

int ozoneAppServe(OzoneAllocatorT* allocator, OzoneAppConfigT config)
{
  OzoneHTTPConfigT http_config = { 0 };
  http_config.port = config.port;

  ozoneLogInfo("Registering %ld routes", config.routes_count);

  OzoneAppApplicationContextT* application_context = ozoneAllocatorReserveOne(allocator, OzoneAppApplicationContextT);

  application_context->route_configs
      = ozoneAllocatorReserveMany(allocator, OzoneAppHTTPRouteConfigT, config.routes_count);
  application_context->route_handler_pipelines
      = ozoneAllocatorReserveMany(allocator, OzoneHTTPHandlerT**, config.routes_count);
  application_context->route_handler_pipelines_counts
      = ozoneAllocatorReserveMany(allocator, size_t, config.routes_count);

  application_context->route_count = config.routes_count;
  for (size_t route_index = 0; route_index < config.routes_count; route_index++) {
    application_context->route_configs[route_index] = config.routes[route_index].config;
    application_context->route_handler_pipelines[route_index]
        = (OzoneHTTPHandlerT**)config.routes[route_index].handler_pipeline;
    application_context->route_handler_pipelines_counts[route_index]
        = config.routes[route_index].handler_pipeline_count;
  }

  http_config.application_context = (void*)application_context;

  OzoneHTTPHandlerT* handler_pipeline[] = { (OzoneHTTPHandlerT*)ozoneAppRouter };
  http_config.handler_pipeline = handler_pipeline;
  http_config.handler_pipeline_count = 1;

  return ozoneHTTPServe(allocator, http_config);
}
