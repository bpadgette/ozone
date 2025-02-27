#include "ozone_string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(OzoneByte)
OZONE_VECTOR_IMPLEMENT_API(OzoneString)

void ozoneStringWriteByte(OzoneAllocator* allocator, OzoneString* string, char byte) {
  if (!string || byte == '\0')
    return;

  char end = '\0';
  OzoneByteVectorPush(allocator, &string->vector, &end);
  if (ozoneVectorLength(&string->vector) < 2)
    OzoneByteVectorPush(allocator, &string->vector, &end);

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
  OzoneString* copy = ozoneAllocatorReserveOne(allocator, OzoneString);
  copy->vector.length = original->vector.length;
  copy->vector.capacity = original->vector.capacity;

  copy->vector.elements = ozoneAllocatorReserveMany(allocator, char, copy->vector.capacity);
  memcpy(copy->vector.elements, original->vector.elements, copy->vector.capacity);

  return copy;
}

void ozoneStringConcatenate(OzoneAllocator* allocator, OzoneString* destination, const OzoneString* source) {
  char* member;
  ozoneVectorForEach(member, &source->vector) { ozoneStringWriteByte(allocator, destination, *member); }
}

void ozoneStringVectorConcatenate(
    OzoneAllocator* allocator, OzoneString* destination, const OzoneStringVector* strings) {
  OzoneString* member;
  ozoneVectorForEach(member, strings) { ozoneStringConcatenate(allocator, destination, member); }
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

  for (size_t string_index = 0; string_index < (ozoneStringLength(string) - ozoneStringLength(search));
       string_index++) {
    if (!memcmp(ozoneStringBuffer(string) + string_index, ozoneStringBuffer(search), ozoneStringLength(search))) {
      return (int)string_index;
    }
  }

  return -1;
}

OzoneString* ozoneStringFromBuffer(OzoneAllocator* allocator, char* buffer, size_t buffer_size) {
  OzoneString* string = ozoneAllocatorReserveOne(allocator, OzoneString);

  size_t real_buffer_size = 0;
  for (; real_buffer_size < buffer_size; real_buffer_size++) {
    if (!buffer[real_buffer_size])
      break;
  }

  string->vector = (OzoneByteVector) {
    .elements = ozoneAllocatorReserveMany(allocator, char, buffer_size + 1),
    .length = real_buffer_size + 1,
    .capacity = real_buffer_size + 1,
  };

  memcpy(string->vector.elements, buffer, real_buffer_size);

  // Only a single null terminator is needed, if the ending of the string is a null terminator we should pop
  while (ozoneStringLength(string) && ozoneStringBufferEnd(string) == '\0') {
    ozoneStringPop(string);
  }

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
