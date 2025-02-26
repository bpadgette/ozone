#ifndef OZONE_SERVE_DIRECTORY_H
#define OZONE_SERVE_DIRECTORY_H
#include "ozone.h"

void ozoneServeDirectory(OzoneAppEvent* event) {
  OzoneString* location = ozoneAppParameter(event, "path:location");
  if (!location) {
    ozoneLogError("Path parameter path:location not specified");
    event->response->code = 503;
    return;
  }

  ozoneAppContextLock(event);
  OzoneString** directory = ozoneAppContextCacheGetRef(event, OzoneString, "option:serve-directory");
  ozoneAppContextUnlock(event);

  if (!directory) {
    ozoneLogError("CLI option serve-directory not specified");
    event->response->code = 503;
    return;
  }

  OzoneString* path = ozoneStringCopy(event->allocator, *directory);
  ozoneStringAppend(event->allocator, path, '/');
  ozoneStringConcatenate(event->allocator, path, location);

  OzoneStringVector file_chunks = (OzoneStringVector) { 0 };
  ozoneFileLoadFromPath(event->allocator, &file_chunks, path, 1024);
  if (!file_chunks.length) {
    event->response->code = 404;
    return;
  }

  ozoneStringJoin(event->allocator, &event->response->body, &file_chunks);

  // TODO: infer Content-Type & chunk HTTP responses
}

#endif
