#include "ozone.h"

#define PAGE_TITLE "My Ozone Server"

// A handler is a function matching the signature `OzoneAppHandler` like
// the function `handleBadRequest` below: `int <name>(OzoneAppEvent*, const OzoneAppContext*)`
int handleBadRequest(OzoneAppEvent* event, OzoneAppContext* context) {
  // context may not be necessary, but handlers should declare it in their
  // signatures for HTTP pipeline memory layout reasons. (void)context
  // is a way to suppress any linter warnings for this unused parameter.
  (void)context;

  // event->response is an OzoneHTTPResponse struct that can be manipulated by handlers
  // without convenience functions as well
  event->response->body = *ozoneString(event->allocator, "No.");
  event->response->code = 400;

  ozoneLogWarn("Bad request at %s", ozoneStringBuffer(&event->request->target));

  return 0;
}

int home(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  event->response->body = *ozoneString(
      event->allocator,
      "<h1>ozone</h1>"
      "<p>Ozone is a minimal dependency, C and web standards based web framework.</p>");

  // Ozone has an HTTP pipelining approach, we may define another handler to handle the
  // event->response->body as the <body> tag of an HTML document at the very end.
  // See `asHTMLDocument` below.

  return 0;
}

int asHTMLDocument(OzoneAppEvent* event, OzoneAppContext* context) {
  // OzoneApp exposes several convenience functions for manipulating app events.
  ozoneAppRenderOzoneShellHTML(event, context, &ozoneStringConstant(PAGE_TITLE), &event->response->body);

  return 0;
}

int setHeaders(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  ozoneAppSetResponseHeader(event, &ozoneStringConstant("X-Server-Name"), &ozoneStringConstant(PAGE_TITLE));

  return 0;
}

int renderHTTPStatusToBody(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  event->response->body = *ozoneHTTPStatusText(event->allocator, event->response->code);

  // Finally, what is going on with the memory allocated by event->allocator?
  // Once a response is written to the socket, all memory reserved with event->allocator is zeroed
  // and made re-usable for the next HTTP pipeline

  return 0;
}

int main() {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take an arbitrary count of handlers
      ozoneAppEndpoint(GET, "/", setHeaders, home, asHTMLDocument),
      ozoneAppEndpoint(POST, "/use-javascript", setHeaders, handleBadRequest, renderHTTPStatusToBody, asHTMLDocument),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", setHeaders, handleBadRequest));

  OzoneStringVector options = ozoneVectorFromElements(
      OzoneString,
      ozoneStringConstant("ozone-js=./build/ozone.js"),
      ozoneStringConstant("ozone-templates-base-path=./include/html"));

  return ozoneAppServe(8080, &endpoints, &options);
}
