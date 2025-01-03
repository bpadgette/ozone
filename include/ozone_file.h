#ifndef OZONE_FILE_H
#define OZONE_FILE_H

#include "ozone_allocator.h"
#include "ozone_string.h"

#include <stdio.h>

OzoneStringTVectorT ozoneFileLoad(
    OzoneAllocatorT* allocator, FILE* file, OzoneStringEncodingT encoding, size_t max_chunk_bytes);
OzoneStringTVectorT ozoneFileLoadFromPath(
    OzoneAllocatorT* allocator, const OzoneStringT* path, OzoneStringEncodingT encoding, size_t max_chunk_bytes);

#endif
