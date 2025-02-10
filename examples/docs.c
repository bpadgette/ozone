#include "ozone.h"

#include "middlewares.h"

void homepage(OzoneAppEvent* event) {
  OzoneStringVector readme_md = ozoneVectorFromElements(OzoneString, ozoneStringConstant("<pre>\n"));
  ozoneFileLoadFromPath(event->allocator, &readme_md, &ozoneStringConstant("./README.md"), 1024);
  ozoneVectorPushOzoneString(event->allocator, &readme_md, &ozoneStringConstant("\n</pre>"));

  ozoneStringJoin(event->allocator, &event->response->body, &readme_md);
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints
      = ozoneVectorFromElements(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", homepage, asHTMLDocument));

  return ozoneAppServe(argc, argv, &endpoints);
}
