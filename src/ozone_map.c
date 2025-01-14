#include "ozone_map.h"

const OzoneString* ozoneStringMapFindValue(const OzoneStringMap* map, const OzoneString* key) {
  if (!map || !key)
    return NULL;

  for (size_t key_index = 0; key_index < ozoneVectorLength(&map->keys) && key_index < ozoneVectorLength(&map->values);
       key_index++) {
    if (!ozoneStringCompare(&ozoneVectorAt(&map->keys, key_index), key))
      return &ozoneVectorAt(&map->values, key_index);
  }

  return NULL;
}

void ozoneStringMapInsert(
    OzoneAllocator* allocator, OzoneStringMap* map, const OzoneString* key, const OzoneString* value) {
  if (!key || !value)
    return;

  for (size_t key_index = 0; key_index < ozoneVectorLength(&map->keys) && key_index < ozoneVectorLength(&map->values);
       key_index++) {
    if (!ozoneStringCompare(&ozoneVectorAt(&map->keys, key_index), key)) {
      map->values.elements[key_index] = *ozoneStringCopy(allocator, value);
      return;
    }
  }

  ozoneVectorPushOzoneString(allocator, &map->keys, ozoneStringCopy(allocator, key));
  ozoneVectorPushOzoneString(allocator, &map->values, ozoneStringCopy(allocator, value));
}
