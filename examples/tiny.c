#include "ozone.h"

void handler(OzoneAppEvent* event) {
  ozoneStringWrite(event->allocator, &event->response->body, "Hello, World! This is how simple Ozone is.");
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVector(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", handler));

  return ozoneAppServe(argc, argv, &endpoints);
}
