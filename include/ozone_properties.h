#ifndef OZONE_PROPERTIES_H
#define OZONE_PROPERTIES_H

#include "ozone_allocator.h"
#include "ozone_file.h"
#include "ozone_map.h"
#include "ozone_string.h"

void ozonePropertiesLoadText(OzoneAllocator* allocator, OzoneStringMap* properties, const OzoneStringVector* text);
void ozonePropertiesLoadFromPath(OzoneAllocator* allocator, OzoneStringMap* properties, const OzoneString* path);

#endif
