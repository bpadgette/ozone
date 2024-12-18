#include "ozone.h"

ozoneHandler(middleware, {
  ozoneHTTPAppendHeader(allocator, &res->headers, ozoneCharArray("X-Server-Name"), ozoneCharArray("My Ozone Server"));
});

ozoneHandler(home, { res->body = ozoneCharArray("Hello, world!"); });
OzoneAppHandlerT* home_stack[] = { middleware, home };

ozoneHandler(badRequest, {
  res->body = ozoneCharArray("I will not");
  res->code = 400;
});
OzoneAppHandlerT* bad_request_stack[] = { middleware, badRequest };

int main()
{
  OzoneAppConfigT config = { .port = 8080 };

  OzoneAppRouteT routes[] = {
    ozoneAppRoute(GET, "/", home_stack),
    ozoneAppRoute(GET, "/use-javascript", bad_request_stack),
  };

  config.routes = routes;
  config.routes_count = 2;

  OzoneAllocatorT* allocator = ozoneAllocatorCreate(4096);
  ozoneAppServe(allocator, config);
  ozoneAllocatorDelete(allocator);

  return 0;
}
