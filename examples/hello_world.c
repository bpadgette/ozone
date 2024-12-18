#include "ozone.h"

ozoneHTTPHandler(middleware, {
  ozoneHTTPAppendHeader(allocator, &res->headers, ozoneCharArray("X-Server-Name"), ozoneCharArray("My Ozone Server"));
});

ozoneHTTPHandler(handler, { res->body = ozoneCharArray("Hello, world!"); });

int main()
{
  OzoneAllocatorT* allocator = ozoneAllocatorCreate(4096);
  OzoneHTTPConfigT config = { 0 };
  config.port = 8080;

  OzoneHTTPHandlerT* handler_pipeline[] = { middleware, handler };
  config.handler_pipeline = handler_pipeline;
  config.handler_pipeline_count = 2;

  return ozoneHTTPServe(allocator, config);
}
