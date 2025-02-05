

#include "ozone_cache.h"

OzoneString* ozoneCacheGet(OzoneAllocator* allocator, OzoneCache* cache, const OzoneString* key) {
  pthread_mutex_lock(&cache->thread_lock);

  OzoneString* value = NULL;
  const OzoneString* _value = ozoneStringMapFindValue(&cache->_map, key);
  if (_value)
    value = ozoneStringCopy(allocator, _value);

  pthread_mutex_unlock(&cache->thread_lock);

  return value;
}

void ozoneCacheSet(OzoneAllocator* allocator, OzoneCache* cache, const OzoneString* key, const OzoneString* value) {
  pthread_mutex_lock(&cache->thread_lock);
  ozoneStringMapInsert(allocator, &cache->_map, key, ozoneStringCopy(allocator, value));
  pthread_mutex_unlock(&cache->thread_lock);
}
