#ifndef OZONE_ARRAY_H
#define OZONE_ARRAY_H
#include <stdint.h>
#include <string.h>
#include "ozone_allocator.h"

typedef struct OzoneArrayString
{
  char *data;
  size_t length;
} OzoneArrayStringT;

OzoneArrayStringT *ozoneArrayStringCreate(OzoneAllocatorT *allocator, size_t size);
#define ozoneArrayStringFromChars(string) \
  ((OzoneArrayStringT){.data = string, .length = sizeof(string)})

int ozoneArrayStringCompare(OzoneArrayStringT *left, OzoneArrayStringT *right);
#define ozoneArrayStringIs(left, right) (ozoneArrayStringCompare(left, right) == 0)

OzoneArrayStringT *ozoneArrayStringCopy(OzoneAllocatorT *allocator, OzoneArrayStringT *source);

#endif
