#include "ozone.h"

ozoneHandler(before, {
  ozoneHTTPAppendHeader(allocator, &res->headers, ozoneCharArray("X-Server-Name"), ozoneCharArray("My Ozone Server"));
});

ozoneHandler(home, { res->body = ozoneCharArray("Hello, world!"); });
OzoneAppHandlerT* home_stack[] = { before, home };

ozoneHandler(badRequest, {
  res->body = ozoneCharArray("I will not");
  res->code = 400;
});
OzoneAppHandlerT* bad_request_stack[] = { before, badRequest };

int main()
{
  OzoneAppRouteT routes[] = {
    ozoneAppRoute(GET, "/", home_stack),
    ozoneAppRoute(GET, "/use-javascript", bad_request_stack),
    ozoneAppRoute(POST, "/just-write-it-in-rust", bad_request_stack),
  };

  OzoneAppConfigT config = {
    .port = 8080,
    .routes = routes,
    .routes_count = 3,
  };

  OzoneAllocatorT* allocator = ozoneAllocatorCreate(4096);
  ozoneAppServe(allocator, config);
  ozoneAllocatorDelete(allocator);

  return 0;
}
