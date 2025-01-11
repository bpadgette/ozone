#include "ozone_string.h"

#include <stdio.h>
#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(OzoneByte)
OZONE_VECTOR_IMPLEMENT_API(OzoneString)
OZONE_VECTOR_IMPLEMENT_API(OzoneStringKeyValue)

void ozoneStringAppend(OzoneAllocator* allocator, OzoneString* string, char byte) {
  if (byte == '\0')
    return;

  if (!ozoneStringLength(string)) {
    char* elements = ozoneAllocatorReserveMany(allocator, char, 2);
    elements[0] = byte;
    elements[1] = '\0';
    string->vector = (OzoneByteVector) { .capacity = 2, .length = 2, .elements = elements };
    return;
  }

  pushOzoneByte(allocator, &string->vector, '\0');
  string->vector.elements[ozoneVectorLength(&string->vector) - 2] = byte;
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

char ozoneStringShift(OzoneString* string) {
  if (ozoneVectorLength(&string->vector) < 2)
    return '\0';

  char shifted = string->vector.elements[0];
  string->vector.elements++;
  string->vector.length--;
  string->vector.capacity--;

  return shifted;
}

OzoneString* ozoneStringCreate(OzoneAllocator* allocator, size_t capacity) {
  char* elements = ozoneAllocatorReserveMany(allocator, char, capacity);
  memset(elements, '\0', capacity);

  OzoneString* string = ozoneAllocatorReserveOne(allocator, OzoneString);
  *string = (OzoneString) { .vector = (OzoneByteVector) { .capacity = capacity, .length = 0, .elements = elements } };

  return string;
}

OzoneString ozoneStringCopy(OzoneAllocator* allocator, const OzoneString* original) {
  char* buffer = ozoneAllocatorReserveMany(allocator, char, ozoneStringLength(original) + 1);
  memcpy(buffer, original->vector.elements, ozoneStringLength(original) + 1);
  return ((OzoneString) {
      .vector = ((OzoneByteVector) {
          .elements = buffer,
          .length = ozoneStringLength(original) + 1,
          .capacity = original->vector.capacity,
      }),
  });
}

void ozoneStringConcatenate(OzoneAllocator* allocator, OzoneString* destination, const OzoneString* source) {
  char* member;
  ozoneVectorForEach(member, &source->vector) { ozoneStringAppend(allocator, destination, *member); }
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

OzoneString ozoneStringFromBuffer(OzoneAllocator* allocator, char* buffer, size_t buffer_size, const OzoneString* end) {
  int location = ozoneStringFindFirst(
      &(OzoneString) {
          .vector = (OzoneByteVector) { .elements = buffer, .length = buffer_size, .capacity = buffer_size },
      },
      end);

  size_t string_length = location == -1 ? buffer_size : ((size_t)location + 1);

  OzoneString string = ((OzoneString) {
      .vector = ((OzoneByteVector) {
          .elements = ozoneAllocatorReserveMany(allocator, char, string_length + 1),
          .length = string_length,
          .capacity = string_length,
      }),
  });

  memcpy(ozoneStringBuffer(&string), buffer, ozoneStringLength(&string) + 1);
  ozoneStringBuffer(&string)[ozoneStringLength(&string)] = '\0';

  return string;
}

OzoneString* ozoneStringKeyValueVectorFind(const OzoneStringKeyValueVector* vector, const OzoneString* key) {
  OzoneStringKeyValue* pair;
  ozoneVectorForEach(pair, vector) {
    if (ozoneStringCompare(&pair->key, key) == 0)
      return &pair->value;
  }

  return NULL;
}
