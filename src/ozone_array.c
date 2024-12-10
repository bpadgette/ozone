#include "ozone_array.h"

#include <math.h>
#include "ozone_log.h"

OzoneArrayStringT *ozoneArrayStringCreate(OzoneAllocatorT *allocator, size_t size)
{
  if (!allocator || size == 0)
    return NULL;

  OzoneArrayStringT *array = ozoneAllocatorReserveOne(allocator, OzoneArrayStringT);
  array->data = ozoneAllocatorReserveMany(allocator, char, size);
  array->length = size;

  return array;
}

int ozoneArrayStringCompare(OzoneArrayStringT *left, OzoneArrayStringT *right)
{
  return strncmp(
      left ? left->data : "\0",
      right ? right->data : "\0",
      fmin(
          left ? left->length : 1,
          right ? right->length : 1));
}

OzoneArrayStringT *ozoneArrayStringCopy(OzoneAllocatorT *allocator, OzoneArrayStringT *source)
{
  if (!source)
    return NULL;

  OzoneArrayStringT *copy = ozoneArrayStringCreate(allocator, source->length);
  if (!copy)
    return NULL;

  for (size_t index = 0; index < source->length; index++)
    copy->data[index] = source->data[index];

  return copy;
}
