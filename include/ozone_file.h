#ifndef OZONE_FILE_H
#define OZONE_FILE_H

#include "ozone_allocator.h"
#include "ozone_string.h"
#include <stdio.h>

void ozoneFileLoad(OzoneAllocator* allocator, OzoneStringVector* destination, FILE* file, size_t max_chunk_bytes);
void ozoneFileLoadFromPath(
    OzoneAllocator* allocator, OzoneStringVector* destination, const OzoneString* path, size_t max_chunk_bytes);

#endif
