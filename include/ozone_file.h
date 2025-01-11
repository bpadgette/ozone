#ifndef OZONE_FILE_H
#define OZONE_FILE_H

#include "ozone_allocator.h"
#include "ozone_string.h"
#include <stdio.h>

OzoneStringVector ozoneFileLoad(OzoneAllocator* allocator, FILE* file, size_t max_chunk_bytes);
OzoneStringVector ozoneFileLoadFromPath(OzoneAllocator* allocator, const OzoneString* path, size_t max_chunk_bytes);

#endif
