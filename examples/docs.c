#include "ozone.h"

#define PAGE_TITLE "Ozone Documentation"

#include "middlewares.h"

int homepage(OzoneAppEvent* event) {
  OzoneStringVector readme_md = ozoneVectorFromElements(OzoneString, ozoneStringConstant("<pre>\n"));
  ozoneFileLoadFromPath(event->allocator, &readme_md, &ozoneStringConstant("./README.md"), 1024);
  ozoneVectorPushOzoneString(event->allocator, &readme_md, &ozoneStringConstant("\n</pre>"));

  ozoneStringJoin(event->allocator, &event->response->body, &readme_md);

  return 0;
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints
      = ozoneVectorFromElements(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", homepage, asHTMLDocument));

  return ozoneAppServe(argc, argv, &endpoints);
}
