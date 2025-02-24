#include "ozone.h"

void handler(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "Hello, World! This is how simple Ozone is.");
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", handler));

  return ozoneAppServe(argc, argv, &endpoints);
}
