#ifndef OZONE_ARRAY_H
#define OZONE_ARRAY_H
#include <stdint.h>
#include <string.h>
#include "allocator.h"

typedef struct OZArrayString
{
  char *data;
  size_t length;
} OZArrayStringT;

OZArrayStringT *ozArrayStringCreate(OZAllocatorT *allocator, size_t size);
#define ozArrayStringFromChars(string) \
  ((OZArrayStringT){.data = string, .length = sizeof(string)})

int ozArrayStringCompare(OZArrayStringT *left, OZArrayStringT *right);
#define ozArrayStringIs(left, right) (ozArrayStringCompare(left, right) == 0)

OZArrayStringT *ozArrayStringCopy(OZAllocatorT *allocator, OZArrayStringT *source);

#endif
