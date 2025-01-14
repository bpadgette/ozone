#include "ozone.h"

#define HOME_TEMPLATE "./examples/resources/hello_world.html"
#define SERVER_NAME "My Ozone Server"

int before(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/html"));
  ozoneAppSetResponseHeader(event, &ozoneStringConstant("X-Server-Name"), &ozoneStringConstant(SERVER_NAME));

  return 0;
}

int home(OzoneAppEvent* event, OzoneAppContext* context) {
  ozoneStringMapInsert(
      event->allocator,
      &context->templates.arguments,
      &ozoneStringConstant("title"),
      &ozoneStringConstant(SERVER_NAME));

  ozoneStringMapInsert(
      event->allocator,
      &context->templates.arguments,
      &ozoneStringConstant("body"),
      &ozoneStringConstant("<h1>ozone</h1>"
                           "<p>Ozone is a minimal dependency, C-based web framework.</p>"));

  event->response->body
      = *ozoneTemplatesRender(event->allocator, &context->templates, &ozoneStringConstant(HOME_TEMPLATE));

  return 0;
}

int refuse(OzoneAppEvent* event, OzoneAppContext* context) {
  (void)context;

  event->response->body = *ozoneString(event->allocator, "No.");
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
    *ozoneTemplatesComponentFromFile(allocator, &ozoneStringConstant(HOME_TEMPLATE)),
  };

  return ozoneAppServe(
      allocator,
      8080,
      &ozoneVectorFromArray(OzoneAppEndpoint, endpoints),
      &ozoneVectorFromArray(OzoneTemplatesComponent, templates));
}
