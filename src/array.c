#include "array.h"
#include "log.h"

OZCharArrayT *ozCharArrayCreate(OZAllocatorT *allocator, size_t size)
{
  if (!allocator || size == 0)
    return NULL;

  OZCharArrayT *array = (OZCharArrayT *)ozAllocatorReserveBytes(allocator, sizeof(OZCharArrayT) + size);
  array->data = sizeof(OZCharArrayT) + (char *)array;
  array->size = size;

  return array;
}
