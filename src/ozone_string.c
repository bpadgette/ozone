#include "ozone_string.h"

#include <stdio.h>
#include <string.h>
OZONE_VECTOR_IMPLEMENT_API(char)
OZONE_VECTOR_IMPLEMENT_API(OzoneString)

void ozoneStringAppend(OzoneAllocator* allocator, OzoneString* string, char byte) {
  if (!ozoneStringLength(string) && byte != '\0') {
    char* elements = ozoneAllocatorReserveMany(allocator, char, 2);
    elements[0] = byte;
    elements[1] = '\0';
    string->vector = (OzoneVectorChar) { .capacity = 2, .length = 2, .elements = elements };
    return;
  }

  ozoneVectorPushchar(allocator, &string->vector, '\0');
  string->vector.elements[string->vector.length - 2] = byte;
}

void ozoneStringClear(OzoneString* string) {
  if (!string || !string->vector.length)
    return;

  string->vector.length = 0;
  memset(&string->vector.elements[0], '\0', string->vector.capacity);
}

char ozoneStringPop(OzoneString* string) {
  if (string->vector.length < 2)
    return '\0';

  size_t last = string->vector.length - 2;
  char popped = string->vector.elements[last];
  string->vector.elements[last] = '\0';
  string->vector.length--;

  return popped;
}

OzoneString* ozoneStringCreate(OzoneAllocator* allocator, size_t capacity) {
  char* elements = ozoneAllocatorReserveMany(allocator, char, capacity);
  memset(elements, '\0', capacity);

  OzoneString* string = ozoneAllocatorReserveOne(allocator, OzoneString);
  *string = (OzoneString) { .encoding = OZONE_STRING_ENCODING_ISO_8859_1,
    .vector = (OzoneVectorChar) { .capacity = capacity, .length = 1, .elements = elements } };

  return string;
}

OzoneString ozoneStringCopy(OzoneAllocator* allocator, const OzoneString* original) {
  char* buffer = ozoneAllocatorReserveMany(allocator, char, ozoneStringLength(original) + 1);
  memcpy(buffer, original->vector.elements, ozoneStringLength(original) + 1);
  return ((OzoneString) {
      .vector = ((OzoneVectorChar) {
          .elements = buffer,
          .length = ozoneStringLength(original) + 1,
          .capacity = original->vector.capacity,
      }),
      .encoding = original->encoding,
  });
}

OzoneString ozoneStringJoin(OzoneAllocator* allocator, const OzoneStringVector* vector, OzoneStringEncoding encoding) {
  OzoneString* member;
  size_t length = 1;
  ozoneVectorForEach(member, vector) { length += ozoneStringLength(member); }

  char* elements = ozoneAllocatorReserveMany(allocator, char, length);
  char* cursor = &elements[0];
  ozoneVectorForEach(member, vector) {
    size_t member_length = ozoneStringLength(member);
    memcpy(cursor, member->vector.elements, member_length);
    cursor += member_length;
  }
  cursor[0] = '\0';

  return (OzoneString) {
    .vector = ((OzoneVectorChar) {
        .elements = elements,
        .length = length,
        .capacity = length,
    }),
    .encoding = encoding,
  };
}

int ozoneStringCompare(const OzoneString* left, const OzoneString* right) {
  if (!left && !right)
    return 0;
  if (!right)
    return -1;
  if (!left)
    return 1;

  // todo: smart encoding comparison. Several encodings are equivalent, i.e. an ASCII string is valid UTF-8
  if (left->encoding < right->encoding)
    return -1;
  if (left->encoding > right->encoding)
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

OzoneString ozoneStringFromBuffer(
    OzoneAllocator* allocator, char* buffer, size_t buffer_size, const OzoneString* end, OzoneStringEncoding encoding) {
  int location = ozoneStringFindFirst(
      &(OzoneString) {
          .encoding = encoding,
          .vector = (OzoneVectorChar) { .elements = buffer, .length = buffer_size, .capacity = buffer_size },
      },
      end);

  size_t string_length = location == -1 ? buffer_size : ((size_t)location + 1);

  OzoneString string = ((OzoneString) {
      .vector = ((OzoneVectorChar) {
          .elements = ozoneAllocatorReserveMany(allocator, char, string_length + 1),
          .length = string_length,
          .capacity = string_length,
      }),
      .encoding = encoding,
  });

  memcpy(ozoneStringBuffer(&string), buffer, ozoneStringLength(&string) + 1);
  ozoneStringBuffer(&string)[ozoneStringLength(&string)] = '\0';

  return string;
}
