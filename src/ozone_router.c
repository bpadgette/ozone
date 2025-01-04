#include "ozone_router.h"

int ozoneRouter(OzoneRouterContext* context) {
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

  OzoneHTTPHandler** handler_pipeline = context->application->route_handler_pipelines[route_index];
  size_t handler_pipeline_count = context->application->route_handler_pipelines_counts[route_index];
  for (size_t handler_index = 0; handler_index < handler_pipeline_count; handler_index++) {
    handler_pipeline[handler_index]((OzoneHTTPContext*)context);
  }

  return 0;
}
