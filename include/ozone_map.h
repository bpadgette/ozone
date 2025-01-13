#ifndef OZONE_MAP_H
#define OZONE_MAP_H

#include "ozone_string.h"

typedef struct OzoneStringMapStruct {
  OzoneStringVector* keys;
  OzoneStringVector* values;
} OzoneStringMap;

OzoneStringMap* ozoneStringMapCreate(OzoneAllocator* allocator);
const OzoneString* ozoneStringMapFindValue(const OzoneStringMap* map, const OzoneString* key);
void ozoneStringMapInsert(
    OzoneAllocator* allocator, OzoneStringMap** map, const OzoneString* key, const OzoneString* value);

#endif
