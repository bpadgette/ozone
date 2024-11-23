#ifndef OZONE_ARRAY_H
#define OZONE_ARRAY_H
#include <stdint.h>
#include "allocator.h"

typedef struct OZArray
{
  char *data;
  size_t size;
} OZArrayT;

OZArrayT *ozArrayCreate(OZAllocatorT *allocator, size_t size);

#endif
