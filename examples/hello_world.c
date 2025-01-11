#include "ozone.h"

#define HOME_TEMPLATE "./examples/resources/hello_world.html"
#define SERVER_NAME "My Ozone Server"

int before(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;
  ozoneAppSetResponseHeader(event, &ozoneString("Content-Type"), &ozoneString("text/html"));
  ozoneAppSetResponseHeader(event, &ozoneString("X-Server-Name"), &ozoneString(SERVER_NAME));

  return 0;
}

int home(OzoneAppEvent* event, OzoneAppContext* context) {
  ozoneStringPushKeyValue(
      event->allocator, &context->templates.arguments, &ozoneString("title"), &ozoneString(SERVER_NAME));

  ozoneStringPushKeyValue(
      event->allocator,
      &context->templates.arguments,
      &ozoneString("body"),
      &ozoneString("<h1>ozone</h1>"
                   "<p>Ozone is a minimal dependency, C-based web framework.</p>"));

  event->response->body = ozoneTemplatesRender(event->allocator, &context->templates, &ozoneString(HOME_TEMPLATE));

  return 0;
}

int refuse(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;
  event->response->body = ozoneString("No.");
  event->response->code = 400;
  return 0;
}

OzoneAppHandler* home_stack[] = { before, home };
OzoneAppHandler* refuse_stack[] = { before, refuse };

int main() {
  OzoneAllocator* allocator = ozoneAllocatorCreate(4096);

  OzoneAppEndpoint endpoints[] = {
    ozoneAppEndpoint(GET, "/", home_stack),
    ozoneAppEndpoint(POST, "/use-javascript", refuse_stack),
    ozoneAppEndpoint(PUT, "/just-write-it-in-rust", refuse_stack),
  };

  OzoneTemplatesComponent templates[] = {
    ozoneTemplatesComponentFromFile(allocator, &ozoneString(HOME_TEMPLATE)),
  };

  return ozoneAppServe(
      allocator, 8080, &ozoneVector(OzoneAppEndpoint, endpoints), &ozoneVector(OzoneTemplatesComponent, templates));
}
