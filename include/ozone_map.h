#ifndef OZONE_MAP_H
#define OZONE_MAP_H

#include "ozone_string.h"

#define OZONE_MAP_DECLARE_API(_type_)                                                                                  \
  typedef struct _type_##MapStruct {                                                                                   \
    OzoneStringVector keys;                                                                                            \
    _type_##Vector values;                                                                                             \
  } _type_##Map;                                                                                                       \
  _type_* _type_##MapFind(_type_##Map* map, const OzoneString* key);                                                   \
  void _type_##MapInsert(OzoneAllocator* allocator, _type_##Map* map, const OzoneString* key, _type_* value);

#define OZONE_MAP_IMPLEMENT_API(_type_)                                                                                \
  _type_* _type_##MapFind(_type_##Map* map, const OzoneString* key) {                                                  \
    if (!map || !key)                                                                                                  \
      return NULL;                                                                                                     \
    for (size_t key_index = 0; key_index < map->keys.length && key_index < map->values.length; key_index++) {          \
      if (!ozoneStringCompare(&ozoneVectorAt(&map->keys, key_index), key))                                             \
        return &ozoneVectorAt(&map->values, key_index);                                                                \
    }                                                                                                                  \
    return NULL;                                                                                                       \
  }                                                                                                                    \
  void _type_##MapInsert(OzoneAllocator* allocator, _type_##Map* map, const OzoneString* key, _type_* value) {         \
    if (!key || !value)                                                                                                \
      return;                                                                                                          \
    for (size_t key_index = 0; key_index < map->keys.length && key_index < map->values.length; key_index++) {          \
      if (!ozoneStringCompare(&ozoneVectorAt(&map->keys, key_index), key)) {                                           \
        map->values.elements[key_index] = *value;                                                                      \
        return;                                                                                                        \
      }                                                                                                                \
    }                                                                                                                  \
    OzoneStringVectorPush(allocator, &map->keys, ozoneStringCopy(allocator, key));                                     \
    _type_##VectorPush(allocator, &map->values, value);                                                                \
  }

OZONE_MAP_DECLARE_API(OzoneString)

#endif
