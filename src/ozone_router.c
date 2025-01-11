#include "ozone_router.h"

int ozoneRouter(OzoneHTTPEvent* event, OzoneRouterConfig* config) {
  OzoneRouterHTTPEndpoint* endpoint;
  ozoneVectorForEach(endpoint, config->endpoints) {
    if (event->request->method != endpoint->config.method)
      continue;

    if (ozoneStringCompare(&event->request->target, &endpoint->config.target_pattern) != 0)
      continue;

    OzoneSocketHandlerRef* handler;
    ozoneVectorForEach(handler, &endpoint->handler_pipeline) {
      (*handler)((OzoneSocketEvent*)event, config->handler_context);
    }
    return 0;
  }

  event->response->code = 404;
  return 0;
}
