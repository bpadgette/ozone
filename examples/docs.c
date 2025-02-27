#include "ozone.h"

#include "example_helpers.h"

void homepage(OzoneAppEvent* event) {
  OzoneStringVector readme_md = ozoneVector(OzoneString, ozoneString("<pre>\n"));
  ozoneFileLoadFromPath(event->allocator, &readme_md, &ozoneString("./README.md"), 1024);
  OzoneStringVectorPush(event->allocator, &readme_md, &ozoneString("\n</pre>"));

  ozoneStringVectorConcatenate(event->allocator, &event->response->body, &readme_md);
}

int main(int argc, char* argv[]) {
  OzoneAppEndpointVector endpoints
      = ozoneVector(OzoneAppEndpoint, ozoneAppEndpoint(GET, "/", homepage, asHTMLDocument));

  return ozoneAppServe(argc, argv, &endpoints);
}
