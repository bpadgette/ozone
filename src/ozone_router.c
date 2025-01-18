#include "ozone_router.h"

int ozoneRouter(OzoneRouterConfig* config, OzoneHTTPEvent* event, void* context) {
  if (!config) {
    event->response->code = 502;
    return 0;
  }

  OzoneRouterHTTPEndpoint* endpoint;
  ozoneVectorForEach(endpoint, &config->endpoints) {
    if (event->request->method != endpoint->config.method)
      continue;

    if (ozoneStringCompare(&event->request->target, &endpoint->config.target_pattern) != 0)
      continue;

    OzoneSocketHandlerRef* handler;
    ozoneVectorForEach(handler, &endpoint->handler_pipeline) { (*handler)((OzoneSocketEvent*)event, context); }
    return 0;
  }

  event->response->code = 404;
  return 0;
}
