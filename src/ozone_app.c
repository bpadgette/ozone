#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

int ozoneAppServe(OzoneAppConfigT config) {
  OzoneHTTPConfigT http_config = { 0 };
  http_config.allocator = config.allocator;
  http_config.port = config.port;

  ozoneLogInfo("Registering %ld routes", config.router.endpoints_count);

  OzoneRouterApplicationContextT* app = ozoneAllocatorReserveOne(config.allocator, OzoneRouterApplicationContextT);

  app->route_configs
      = ozoneAllocatorReserveMany(config.allocator, OzoneRouterHTTPConfigT, config.router.endpoints_count);
  app->route_handler_pipelines
      = ozoneAllocatorReserveMany(config.allocator, OzoneHTTPHandlerT**, config.router.endpoints_count);
  app->route_handler_pipelines_counts
      = ozoneAllocatorReserveMany(config.allocator, size_t, config.router.endpoints_count);

  app->route_count = config.router.endpoints_count;
  for (size_t route_index = 0; route_index < config.router.endpoints_count; route_index++) {
    app->route_configs[route_index] = config.router.endpoints[route_index].config;
    app->route_handler_pipelines[route_index]
        = (OzoneHTTPHandlerT**)config.router.endpoints[route_index].handler_pipeline;
    app->route_handler_pipelines_counts[route_index] = config.router.endpoints[route_index].handler_pipeline_count;
  }

  http_config.application = (void*)app;

  OzoneHTTPHandlerT* handler_pipeline[] = { (OzoneHTTPHandlerT*)ozoneRouter };
  http_config.handler_pipeline = handler_pipeline;
  http_config.handler_pipeline_count = 1;

  return ozoneHTTPServe(http_config);
}
