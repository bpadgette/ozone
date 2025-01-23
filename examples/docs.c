#include "ozone.h"

#define PAGE_TITLE "Ozone Documentation"

int homepage(OzoneAppEvent* event, OzoneAppContext* context) {
  OzoneStringVector readme_md = ozoneVectorFromElements(OzoneString, ozoneStringConstant("<pre>\n"));
  ozoneFileLoadFromPath(event->allocator, &readme_md, &ozoneStringConstant("./README.md"), 1024);
  ozoneVectorPushOzoneString(event->allocator, &readme_md, &ozoneStringConstant("\n</pre>"));

  ozoneAppRenderOzoneShellHTML(
      event, context, &ozoneStringConstant(PAGE_TITLE), ozoneStringJoin(event->allocator, &readme_md));

  return 0;
}

int main(void) {
  OzoneAppEndpointVector endpoints = ozoneVectorFromElements(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", homepage));

  OzoneStringVector options
      = ozoneVectorFromElements(OzoneString, ozoneStringConstant("ozone-templates-base-path=./include/html"));

  return ozoneAppServe(8080, &endpoints, &options);
}
