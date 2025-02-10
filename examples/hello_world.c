#include "ozone.h"

#include "middlewares.h"

#define PAGE_TITLE "My Ozone Server"

void setHeaders(OzoneAppEvent* event) {
  ozoneMapInsertOzoneString(
      event->allocator,
      &event->response->headers,
      &ozoneStringConstant("X-Server-Name"),
      &ozoneStringConstant(PAGE_TITLE));
}

void handleBadRequest(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "No.");
  event->response->code = 400;

  ozoneLogWarn("Bad request at %s", ozoneStringBuffer(&event->request->target));
}

void home(OzoneAppEvent* event) {
  event->response->body = *ozoneString(
      event->allocator,
      "<h1>ozone</h1>"
      "<p>Ozone is a minimal dependency, C and web standards based web framework.</p>"
      "<a href=\"/greet/World\">/greet/World</a></li>");
}

void greet(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "<a href=\"/\">Home</a><h1>Hello, ");

  OzoneString* name = ozoneMapGetOzoneString(&event->parameters, &ozoneStringConstant("path:nickname"));
  if (!name) {
    name = ozoneMapGetOzoneString(&event->parameters, &ozoneStringConstant("path:name"));
  }

  if (!name) {
    name = &ozoneStringConstant("World");
  }

  ozoneStringConcatenate(event->allocator, &event->response->body, name);
  ozoneStringConcatenate(event->allocator, &event->response->body, &ozoneStringConstant("!</h1>"));
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take an arbitrary count of handlers
      ozoneAppEndpoint(GET, "/", setHeaders, home, asHTMLDocument),
      ozoneAppEndpoint(GET, "/greet/{name}", setHeaders, greet, asHTMLDocument),
      ozoneAppEndpoint(GET, "/greet/{name}/secret/{nickname}", setHeaders, greet, asHTMLDocument),
      ozoneAppEndpoint(POST, "/use-javascript", setHeaders, handleBadRequest, asHTMLDocument),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", setHeaders, handleBadRequest));

  return ozoneAppServe(argc, argv, &endpoints);
}
