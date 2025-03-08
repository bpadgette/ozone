#include "ozone.h"

#include "example_helpers.h"

// This demo is hosted at https://ozone.bpadgette.net

void home(OzoneAppEvent* event) {
  ozoneStringWrite(
      event->allocator,
      &event->response->body,
      "<h1>ozone</h1>"
      "<p>Ozone is a minimal dependency, C-based web framework. (<a "
      "href=\"https://github.com/bpadgette/ozone\">Github Repository</a>)</p>"
      "<ul>");

  OzoneStringVector links = ozoneVector(
      OzoneString,
      ozoneString("/assets/LoremIpsum.txt"),
      ozoneString("/assets/style.css"),
      ozoneString("/greet/World"),
      ozoneString("/greet/there"));

  // TODO: text-templating with ozone_template should have iterative blocks
  OzoneString* link;
  ozoneVectorForEach(link, &links) {
    ozoneStringWrite(event->allocator, &event->response->body, "<li><a href=\"");
    ozoneStringConcatenate(event->allocator, &event->response->body, link);

    if (ozoneStringFindFirst(link, &ozoneString("/assets/")) == 0)
      ozoneStringWrite(event->allocator, &event->response->body, "\" target=\"_blank\">");
    else
      ozoneStringWrite(event->allocator, &event->response->body, "\">");

    ozoneStringConcatenate(event->allocator, &event->response->body, link);
    ozoneStringWrite(event->allocator, &event->response->body, "</a></li>");
  }

  ozoneStringWrite(
      event->allocator,
      &event->response->body,
      "</ul>"
      "<p>You are viewing <a target=\"_blank\""
      "href=\"https://github.com/bpadgette/ozone/blob/main/examples/hello_world.c\">https://"
      "github.com/bpadgette/ozone/blob/main/examples/hello_world.c</a></p>");
}

void greet(OzoneAppEvent* event) {
  ozoneTemplateWrite(
      event->allocator,
      &event->response->body,
      ozoneTemplate(
          event->allocator,
          "<a href=\"/\">Home</a>"
          "<h1>Hello, {{ path:name }}!</h1>"),
      &event->parameters);
}

void handleBadRequest(OzoneAppEvent* event) {
  ozoneStringWrite(event->allocator, &event->response->body, "No.");
  event->response->code = 400;
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVector(
      OzoneAppEndpoint,
      // Endpoints can take a positive count of handlers
      ozoneAppEndpoint(GET, "/", home, asHTMLDocument),
      ozoneAppEndpoint(GET, "/assets/{location}", ozoneServeDirectory),
      ozoneAppEndpoint(GET, "/greet/{name}", greet, asHTMLDocument),
      ozoneAppEndpoint(POST, "/use-javascript", handleBadRequest),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", handleBadRequest));

  return ozoneAppServe(argc, argv, &endpoints);
}
