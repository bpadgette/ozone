#include "array.h"

#include <math.h>
#include "log.h"

OZArrayStringT *ozArrayStringCreate(OZAllocatorT *allocator, size_t size)
{
  if (!allocator || size == 0)
    return NULL;

  OZArrayStringT *array = (OZArrayStringT *)ozAllocatorReserveBytes(allocator, sizeof(OZArrayStringT) + size);
  array->data = sizeof(OZArrayStringT) + (char *)array;
  array->length = size;

  return array;
}

int ozArrayStringCompare(OZArrayStringT *left, OZArrayStringT *right)
{
  return strncmp(
      left ? left->data : "\0",
      right ? right->data : "\0",
      fmin(
          left ? left->length : 1,
          right ? right->length : 1));
}

OZArrayStringT *ozArrayStringCopy(OZAllocatorT *allocator, OZArrayStringT *source)
{
  if (!source)
    return NULL;

  OZArrayStringT *copy = ozArrayStringCreate(allocator, source->length);
  if (!copy)
    return NULL;

  for (size_t index = 0; index <= source->length; index++)
    copy->data[index] = source->data[index];

  return copy;
}
