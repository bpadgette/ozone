#include "array.h"
#include "log.h"

OZArrayT *ozArrayCreate(OZAllocatorT *allocator, size_t size)
{
  if (!allocator || size == 0)
    return NULL;

  OZArrayT *array = (OZArrayT *)ozAllocatorReserveBytes(allocator, sizeof(OZArrayT) + size);
  array->data = sizeof(OZArrayT) + (char *)array;
  array->size = size;

  return array;
}
