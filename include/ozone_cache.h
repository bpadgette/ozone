#ifndef OZONE_CACHE_H
#define OZONE_CACHE_H

#include "ozone_map.h"
#include <pthread.h>

typedef struct OzoneCacheStruct {
  pthread_mutex_t thread_lock;
  OzoneStringMap _map;
} OzoneCache;

OzoneString* ozoneCacheGet(OzoneAllocator* allocator, OzoneCache* cache, const OzoneString* key);
void ozoneCacheSet(OzoneAllocator* allocator, OzoneCache* cache, const OzoneString* key, const OzoneString* value);

#endif
