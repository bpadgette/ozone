#include "ozone_app.h"

#include "ozone_log.h"
#include "ozone_router.h"

int ozoneAppServe(OzoneAppConfig config) {
  OzoneHTTPConfig http_config = { 0 };
  http_config.allocator = config.allocator;
  http_config.port = config.port;

  ozoneLogInfo("Registering %ld routes", config.router.endpoints_count);

  OzoneRouterApplicationContext* app = ozoneAllocatorReserveOne(config.allocator, OzoneRouterApplicationContext);

  app->route_configs
      = ozoneAllocatorReserveMany(config.allocator, OzoneRouterHTTPConfig, config.router.endpoints_count);
  app->route_handler_pipelines
      = ozoneAllocatorReserveMany(config.allocator, OzoneHTTPHandler**, config.router.endpoints_count);
  app->route_handler_pipelines_counts
      = ozoneAllocatorReserveMany(config.allocator, size_t, config.router.endpoints_count);

  app->route_count = config.router.endpoints_count;
  for (size_t route_index = 0; route_index < config.router.endpoints_count; route_index++) {
    app->route_configs[route_index] = config.router.endpoints[route_index].config;
    app->route_handler_pipelines[route_index]
        = (OzoneHTTPHandler**)config.router.endpoints[route_index].handler_pipeline;
    app->route_handler_pipelines_counts[route_index] = config.router.endpoints[route_index].handler_pipeline_count;
  }

  http_config.application = (void*)app;

  OzoneHTTPHandler* handler_pipeline[] = { (OzoneHTTPHandler*)ozoneRouter };
  http_config.handler_pipeline = handler_pipeline;
  http_config.handler_pipeline_count = 1;

  return ozoneHTTPServe(http_config);
}
