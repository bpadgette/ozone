#ifndef OZONE_SERVE_DIRECTORY_H
#define OZONE_SERVE_DIRECTORY_H
#include "../ozone.h"

#define OZONE_SERVE_DIRECTORY_CONTENT_TYPES "middlewares:serve-directory:content-types"
#define OZONE_SERVE_DIRECTORY_DEFAULT_CONTENT_TYPE "application/octet-stream"

void ozoneServeDirectory(OzoneAppEvent* event) {
  OzoneString* location = ozoneAppParameter(event, "path:location");
  if (!location) {
    ozoneLogError("Path parameter path:location not specified");
    event->response->code = 503;
    return;
  }

  ozoneAppContextLock(event);
  OzoneString** directory = ozoneAppContextCacheGetRef(event, OzoneString, "option:serve-directory");
  if (!directory) {
    ozoneAppContextUnlock(event);
    ozoneLogError("CLI option serve-directory not specified");
    event->response->code = 503;
    return;
  }

  OzoneString* path = ozoneStringCopy(event->allocator, *directory);
  ozoneAppContextUnlock(event);

  ozoneStringWriteByte(event->allocator, path, '/');
  ozoneStringConcatenate(event->allocator, path, location);

  OzoneStringVector file_chunks = (OzoneStringVector) { 0 };
  ozoneFileLoadFromPath(event->allocator, &file_chunks, path, 1024);
  if (!file_chunks.length) {
    event->response->code = 404;
    return;
  }

  // TODO: efficiently chunk HTTP responses
  ozoneStringVectorConcatenate(event->allocator, &event->response->body, &file_chunks);

  int extension_start = 1 + ozoneStringFindLast(path, &ozoneString("."));
  OzoneString* extension = ozoneStringAllocate(event->allocator, "");
  while (extension_start < (int)ozoneStringLength(path)) {
    ozoneStringWriteByte(event->allocator, extension, ozoneStringBufferAt(path, extension_start));
    extension_start++;
  }

  OzoneString* content_type = ozoneStringAllocate(event->allocator, OZONE_SERVE_DIRECTORY_DEFAULT_CONTENT_TYPE);
  OzoneStringMapInsert(event->allocator, &event->response->headers, &ozoneString("Content-Type"), content_type);

  ozoneAppContextLock(event);
  OzoneString** content_types_path = ozoneAppContextCacheGetRef(event, OzoneString, "option:content-types");
  if (!content_types_path) {
    ozoneAppContextUnlock(event);
    ozoneLogWarn(
        "CLI option content-types not specified, all files will be "
        "assigned " OZONE_SERVE_DIRECTORY_DEFAULT_CONTENT_TYPE);
    return;
  }

  OzoneStringMap** content_types
      = ozoneAppContextCacheGetRef(event, OzoneStringMap, OZONE_SERVE_DIRECTORY_CONTENT_TYPES);
  if (!content_types) {
    OzoneStringMap* content_types_map = ozoneAllocatorReserveOne(event->context->allocator, OzoneStringMap);
    ozonePropertiesLoadFromPath(event->context->allocator, content_types_map, *content_types_path);
    content_types = &content_types_map;

    OzoneAppVoidRefMapInsert(
        event->context->allocator,
        event->context->cache,
        &ozoneString(OZONE_SERVE_DIRECTORY_CONTENT_TYPES),
        (OzoneAppVoidRef*)content_types);
  }

  content_type = OzoneStringMapFind(*content_types, extension);
  if (content_type)
    OzoneStringMapInsert(event->allocator, &event->response->headers, &ozoneString("Content-Type"), content_type);

  ozoneAppContextUnlock(event);
}

#endif
