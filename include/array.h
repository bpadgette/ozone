#ifndef OZONE_ARRAY_H
#define OZONE_ARRAY_H
#include <stdint.h>
#include "allocator.h"

typedef struct OZCharArray
{
  char *data;
  size_t size;
} OZCharArrayT;

OZCharArrayT *ozCharArrayCreate(OZAllocatorT *allocator, size_t size);

#endif
