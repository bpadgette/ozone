#include "ozone.h"

#include "middlewares.h"

#define PAGE_TITLE "My Ozone Server"

void setHeaders(OzoneAppEvent* event) {
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("X-Server-Name"), &ozoneStringConstant(PAGE_TITLE));
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
      "<p>Ozone is a minimal dependency, C and web standards based web framework.</p>");
}

void renderHTTPStatusToBody(OzoneAppEvent* event) {
  // ozone modules like ozone_http expose helpful functions too
  event->response->body = *ozoneHTTPStatusText(event->allocator, event->response->code);
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take an arbitrary count of handlers
      ozoneAppEndpoint(GET, "/", setHeaders, home, asHTMLDocument),
      ozoneAppEndpoint(POST, "/use-javascript", setHeaders, handleBadRequest, renderHTTPStatusToBody, asHTMLDocument),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", setHeaders, handleBadRequest));

  return ozoneAppServe(argc, argv, &endpoints);
}
