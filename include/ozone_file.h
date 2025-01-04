#ifndef OZONE_FILE_H
#define OZONE_FILE_H

#include "ozone_allocator.h"
#include "ozone_string.h"

#include <stdio.h>

OzoneStringVector ozoneFileLoad(
    OzoneAllocator* allocator, FILE* file, OzoneStringEncoding encoding, size_t max_chunk_bytes);
OzoneStringVector ozoneFileLoadFromPath(
    OzoneAllocator* allocator, const OzoneString* path, OzoneStringEncoding encoding, size_t max_chunk_bytes);

#endif
