#include "ozone.h"

ozoneHTTPHandler(before, {
  ozoneHTTPAppendHeader(allocator, &res->headers, ozoneCharArray("X-Server-Name"), ozoneCharArray("My Ozone Server"));
});

ozoneHTTPHandler(home, { res->body = ozoneCharArray("Hello, world!"); });
OzoneHTTPHandlerT* home_stack[] = { before, home };

ozoneHTTPHandler(badRequest, {
  res->body = ozoneCharArray("I will not");
  res->code = 400;
});
OzoneHTTPHandlerT* bad_request_stack[] = { before, badRequest };

int main() {
  OzoneRouterHTTPEndpointT endpoints[] = {
    ozoneHTTPEndpoint(GET, "/", home_stack),
    ozoneHTTPEndpoint(GET, "/use-javascript", bad_request_stack),
    ozoneHTTPEndpoint(POST, "/just-write-it-in-rust", bad_request_stack),
  };

  OzoneAppConfigT config = { .port = 8080, .router = ozoneRouterConfig(endpoints) };

  OzoneAllocatorT* allocator = ozoneAllocatorCreate(4096);
  int return_code = ozoneAppServe(allocator, config);
  ozoneAllocatorDelete(allocator);

  return return_code;
}
