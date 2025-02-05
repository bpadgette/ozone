#include "ozone.h"

#define PAGE_TITLE "My Ozone Server"

int setHeaders(OzoneAppEvent* event) {
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("X-Server-Name"), &ozoneStringConstant(PAGE_TITLE));

  return 0;
}

int handleBadRequest(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "No.");
  event->response->code = 400;

  ozoneLogWarn("Bad request at %s", ozoneStringBuffer(&event->request->target));

  return 0;
}

int home(OzoneAppEvent* event) {
  event->response->body = *ozoneString(
      event->allocator,
      "<h1>ozone</h1>"
      "<p>Ozone is a minimal dependency, C and web standards based web framework.</p>");

  return 0;
}

int asHTMLDocument(OzoneAppEvent* event) {
  // deprecated: I think the shell file should be application specific, I will remove this from the framework
  ozoneAppRenderOzoneShellHTML(event, &ozoneStringConstant(PAGE_TITLE));

  return 0;
}

int renderHTTPStatusToBody(OzoneAppEvent* event) {
  // ozone modules like ozone_http expose helpful functions too
  event->response->body = *ozoneHTTPStatusText(event->allocator, event->response->code);

  return 0;
}

int main(void) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take an arbitrary count of handlers
      ozoneAppEndpoint(GET, "/", setHeaders, home, asHTMLDocument),
      ozoneAppEndpoint(POST, "/use-javascript", setHeaders, handleBadRequest, renderHTTPStatusToBody, asHTMLDocument),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", setHeaders, handleBadRequest));

  OzoneStringVector options
      = ozoneVectorFromElements(OzoneString, ozoneStringConstant("ozone-templates-base-path=./include/html"));

  return ozoneAppServe(8080, &endpoints, &options);
}
