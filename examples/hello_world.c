#include "ozone.h"

#define HOME_TEMPLATE "./examples/resources/hello_world.html"
#define SERVER_NAME "My Ozone Server"

// A handler is a function matching the signature `OzoneAppHandler` like
// the function `home` below: `int <name>(OzoneAppEvent*, OzoneAppContext*)`
int home(OzoneAppEvent* event, OzoneAppContext* context) {

  // Ozone is designed with zero-is-initialization (ZII) principles in mind, so
  // a struct without an explicit (type)Create function is safe to zero init.
  OzoneStringMap template_args = { 0 };

  ozoneStringMapInsert(
      event->allocator, &template_args, &ozoneStringConstant("title"), &ozoneStringConstant(SERVER_NAME));
  ozoneStringMapInsert(
      event->allocator,
      &template_args,
      &ozoneStringConstant("body"),
      &ozoneStringConstant("<h1>ozone</h1>"
                           "<p>Ozone is a minimal dependency, C-based web framework.</p>"));

  // OzoneApp exposes several convenience functions for manipulating app events.

  // The template used here is already in memory, so no file is opened while handling, go to
  // main to see how the templates are configured and added to the OzoneAppContext.
  ozoneAppRenderResponseBody(event, context, &ozoneStringConstant(HOME_TEMPLATE), &template_args);

  return 0;
}

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

int setHeaders(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/html"));
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("X-Server-Name"), &ozoneStringConstant(SERVER_NAME));

  return 0;
}

int renderHTTPStatusToBody(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  event->response->body = *ozoneHTTPStatusText(event->allocator, event->response->code);

  // What is going on with the memory allocated by event->allocator?
  // Once a response is written to the socket, all memory reserved with event->allocator is zeroed
  // and made re-usable for the next HTTP pipeline

  return 0;
}

int main() {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take an arbitrary count of handlers
      ozoneAppEndpoint(GET, "/", setHeaders, home),
      ozoneAppEndpoint(POST, "/use-javascript", setHeaders, handleBadRequest, renderHTTPStatusToBody),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", setHeaders, handleBadRequest));

  OzoneStringVector templates = ozoneVectorFromElements(OzoneString, ozoneStringConstant(HOME_TEMPLATE));

  return ozoneAppServe(8080, &endpoints, &templates);
}
