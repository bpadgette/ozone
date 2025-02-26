#include "ozone.h"

#include "example_helpers.h"

// This demo is hosted at https://ozone.bpadgette.net

void home(OzoneAppEvent* event) {
  OzoneStringVector body = ozoneVectorFromElements(
      OzoneString,
      ozoneStringConstant("<h1>ozone</h1>"
                          "<p>Ozone is a minimal dependency, C-based web framework. (<a "
                          "href=\"https://github.com/bpadgette/ozone\">Github Repository</a>)</p>"
                          "<ul>"));

  OzoneStringVector links = ozoneVectorFromElements(
      OzoneString,
      ozoneStringConstant("/assets/LoremIpsum.txt"),
      ozoneStringConstant("/greet/World"),
      ozoneStringConstant("/greet/World/secret/there"));

  // TODO: text-templating with ozone_templates should have iterative blocks
  OzoneString* link;
  ozoneVectorForEach(link, &links) {
    ozoneVectorPushOzoneString(event->allocator, &body, &ozoneStringConstant("<li><a href=\""));
    ozoneVectorPushOzoneString(event->allocator, &body, link);
    if (ozoneStringFindFirst(link, &ozoneStringConstant("/assets/")) == 0) {
      ozoneVectorPushOzoneString(event->allocator, &body, &ozoneStringConstant("\" target=\"_blank\">"));
    } else {
      ozoneVectorPushOzoneString(event->allocator, &body, &ozoneStringConstant("\">"));
    }
    ozoneVectorPushOzoneString(event->allocator, &body, link);
    ozoneVectorPushOzoneString(event->allocator, &body, &ozoneStringConstant("</a></li>"));
  }

  ozoneVectorPushOzoneString(
      event->allocator,
      &body,
      &ozoneStringConstant("</ul>"
                           "<p>You are viewing <a target=\"_blank\""
                           "href=\"https://github.com/bpadgette/ozone/blob/main/examples/hello_world.c\">https://"
                           "github.com/bpadgette/ozone/blob/main/examples/hello_world.c</a></p>"));

  ozoneStringJoin(event->allocator, &event->response->body, &body);
}

void greet(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "<a href=\"/\">Home</a><h1>Hello, ");

  OzoneString* name = ozoneAppParameter(event, "path:nickname");
  if (!name)
    name = ozoneAppParameter(event, "path:name");

  if (!name)
    name = ozoneString(event->allocator, "World");

  ozoneStringConcatenate(event->allocator, &event->response->body, name);
  ozoneStringConcatenate(event->allocator, &event->response->body, &ozoneStringConstant("!</h1>"));
}

void handleBadRequest(OzoneAppEvent* event) {
  event->response->body = *ozoneString(event->allocator, "No.");
  event->response->code = 400;

  ozoneLogWarn("Bad request at %s", ozoneStringBuffer(&event->request->target));
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(
      OzoneAppEndpoint,
      // Endpoints can take a positive count of handlers
      ozoneAppEndpoint(GET, "/", home, asHTMLDocument),
      ozoneAppEndpoint(GET, "/assets/{location}", ozoneServeDirectory),
      ozoneAppEndpoint(GET, "/greet/{name}", greet, asHTMLDocument),
      ozoneAppEndpoint(GET, "/greet/{name}/secret/{nickname}", greet, asHTMLDocument),
      ozoneAppEndpoint(POST, "/use-javascript", handleBadRequest),
      ozoneAppEndpoint(PUT, "/just-write-it-in-rust", handleBadRequest));

  return ozoneAppServe(argc, argv, &endpoints);
}
