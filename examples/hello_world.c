#include "ozone.h"

#define HOME_TEMPLATE "./examples/resources/hello_world.html"
#define SERVER_NAME "My Ozone Server"

// A handler is function matching the signature OzoneAppHandler, it meets the criteria of
// the function below: int <name>(OzoneAppEvent*, OzoneAppContext*)
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

  // OzoneApp exposes several convenience functions for manipulating app events
  ozoneAppRenderResponseBody(event, context, &ozoneStringConstant(HOME_TEMPLATE), &template_args);

  return 0;
}

int refuse(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  // event->response is a structured HTTP response that can be manipulated by handlers
  event->response->body = *ozoneString(event->allocator, "No.");
  event->response->code = 400;

  return 0;
}

int before(OzoneAppEvent* event, OzoneAppContext* context) {
  // context may not be necessary, but handlers should declare it in their
  // signatures for HTTP pipeline memory layout reasons
  (void)context;

  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/html"));
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("X-Server-Name"), &ozoneStringConstant(SERVER_NAME));

  return 0;
}

int renderHTTPStatus(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  event->response->body = *ozoneHTTPStatusText(event->allocator, event->response->code);

  return 0;
}

int main() {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take an arbitrary count of handlers
      ozoneAppEndpoint(GET, "/", before, home),
      ozoneAppEndpoint(POST, "/use-javascript", before, refuse, renderHTTPStatus),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", before, refuse));

  OzoneStringVector templates = ozoneVectorFromElements(OzoneString, ozoneStringConstant(HOME_TEMPLATE));

  return ozoneAppServe(8080, &endpoints, &templates);
}
