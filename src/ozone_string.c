#include "ozone_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(OzoneByte)
OZONE_VECTOR_IMPLEMENT_API(OzoneString)

void ozoneStringAppend(OzoneAllocator* allocator, OzoneString* string, char byte) {
  if (!string || byte == '\0')
    return;

  if (!ozoneStringLength(string)) {
    OzoneByteVector* vector = ozoneAllocatorReserveOne(allocator, OzoneByteVector);
    vector->length = 2;
    vector->capacity = 2;
    vector->elements = ozoneAllocatorReserveMany(allocator, char, vector->capacity);
    vector->elements[0] = byte;

    string->vector = *vector;
    return;
  }

  char* end = "\0";
  ozoneVectorPushOzoneByte(allocator, &string->vector, end);
  string->vector.elements[string->vector.length - 2] = byte;
}

void ozoneStringClear(OzoneString* string) {
  if (!string || !ozoneVectorLength(&string->vector))
    return;

  string->vector.length = 0;
  memset(string->vector.elements, '\0', string->vector.capacity);
}

char ozoneStringPop(OzoneString* string) {
  if (ozoneVectorLength(&string->vector) < 2)
    return '\0';

  size_t last = ozoneVectorLength(&string->vector) - 2;
  char popped = string->vector.elements[last];
  string->vector.elements[last] = '\0';
  string->vector.length--;

  return popped;
}

OzoneString* ozoneStringSlice(OzoneAllocator* allocator, OzoneString* string, size_t begin, size_t end) {
  return ozoneStringFromBuffer(allocator, &ozoneVectorAt(&string->vector, begin), end - begin);
}

OzoneString* ozoneStringCopy(OzoneAllocator* allocator, const OzoneString* original) {
  OzoneByteVector* vector = ozoneAllocatorReserveOne(allocator, OzoneByteVector);
  vector->elements = ozoneAllocatorReserveMany(allocator, char, original->vector.capacity);
  vector->capacity = original->vector.capacity;
  vector->length = original->vector.length;

  memcpy(vector->elements, original->vector.elements, vector->capacity);

  OzoneString* copy = ozoneAllocatorReserveOne(allocator, OzoneString);
  copy->vector = *vector;

  return copy;
}

void ozoneStringConcatenate(OzoneAllocator* allocator, OzoneString* destination, const OzoneString* source) {
  char* member;
  ozoneVectorForEach(member, &source->vector) { ozoneStringAppend(allocator, destination, *member); }
}

OzoneString* ozoneStringJoin(OzoneAllocator* allocator, const OzoneStringVector* strings) {
  OzoneString* string = ozoneString(allocator, "");

  OzoneString* member;
  ozoneVectorForEach(member, strings) { ozoneStringConcatenate(allocator, string, member); }

  return string;
}

int ozoneStringCompare(const OzoneString* left, const OzoneString* right) {
  if (!left && !right)
    return 0;
  if (!right)
    return -1;
  if (!left)
    return 1;

  if (ozoneStringLength(left) < ozoneStringLength(right))
    return -1;
  if (ozoneStringLength(left) > ozoneStringLength(right))
    return 1;

  return memcmp(ozoneStringBuffer(left), ozoneStringBuffer(right), ozoneStringLength(left) + 1);
}

int ozoneStringFindFirst(const OzoneString* string, const OzoneString* search) {
  if (!search || !string)
    return -1;

  for (size_t string_index = 0; string_index < ozoneStringLength(string); string_index++) {
    if (!memcmp(ozoneStringBuffer(string) + string_index, ozoneStringBuffer(search), ozoneStringLength(search))) {
      return (int)string_index;
    }
  }

  return -1;
}

OzoneString* ozoneStringFromBuffer(OzoneAllocator* allocator, char* buffer, size_t buffer_size) {
  OzoneByteVector vector = (OzoneByteVector) {
    .elements = ozoneAllocatorReserveMany(allocator, char, buffer_size + 1),
    .length = buffer_size + 1,
    .capacity = buffer_size + 1,
  };

  memcpy(vector.elements, buffer, buffer_size);

  OzoneString* string = ozoneAllocatorReserveOne(allocator, OzoneString);
  string->vector = vector;

  return string;
}

OzoneString* ozoneStringFromInteger(OzoneAllocator* allocator, long int input) {
  char integer_buffer[32] = { 0 };
  snprintf(integer_buffer, sizeof(integer_buffer), "%ld", input);
  OzoneString* integer_string = ozoneStringFromBuffer(allocator, integer_buffer, sizeof(integer_buffer));

  while (!ozoneStringBufferEnd(integer_string))
    ozoneStringPop(integer_string);

  return integer_string;
}

long int ozoneStringToInteger(const OzoneString* input) {
  return input ? strtol(ozoneStringBuffer(input), NULL, 10) : 0;
}
