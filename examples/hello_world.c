#include "ozone.h"

ozoneHTTPHandler(before, {
  ozoneHTTPAppendHeader(allocator, &res->headers, &ozoneString("X-Server-Name"), &ozoneString("My Ozone Server"));
});

ozoneHTTPHandler(home, { res->body = ozoneString("Hello, world!"); });
OzoneHTTPHandlerT* home_stack[] = { before, home };

ozoneHTTPHandler(badRequest, {
  res->body = ozoneString("I will not");
  res->code = 400;
});
OzoneHTTPHandlerT* bad_request_stack[] = { before, badRequest };

int main() {
  OzoneRouterHTTPEndpointT endpoints[] = {
    ozoneHTTPEndpoint(GET, "/", home_stack),
    ozoneHTTPEndpoint(GET, "/use-javascript", bad_request_stack),
    ozoneHTTPEndpoint(POST, "/just-write-it-in-rust", bad_request_stack),
  };

  OzoneAppConfigT config = {
    .allocator = ozoneAllocatorCreate(4096),
    .port = 8080,
    .router = ozoneRouterConfig(endpoints),
  };

  int return_code = ozoneAppServe(config);

  ozoneAllocatorDelete(config.allocator);

  return return_code;
}
