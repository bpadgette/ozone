#include "ozone_string.h"

#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(char)
OZONE_VECTOR_IMPLEMENT_API(OzoneString)

OzoneString ozoneStringCopy(OzoneAllocator* allocator, const OzoneString* original) {
  char* buffer = ozoneAllocatorReserveMany(allocator, char, ozoneStringLength(original) + 1);
  memcpy(buffer, original->vector.elements, ozoneStringLength(original) + 1);
  return ((OzoneString) {
      .vector = ((OzoneVectorChar) {
          .elements = buffer,
          .length = ozoneStringLength(original) + 1,
          .capacity = original->vector.capacity,
          .capacity_increment = original->vector.capacity_increment,
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
        .capacity_increment = length,
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

OzoneString ozoneStringScanBuffer(OzoneAllocator* allocator, char* buffer, size_t buffer_size, const OzoneString* stop,
    OzoneStringEncoding encoding) {
  size_t scan_length = 0;
  while (scan_length < buffer_size) {
    if (stop && (buffer_size - scan_length) > ozoneStringLength(stop)
        && !memcmp(buffer + scan_length, ozoneStringBuffer(stop), ozoneStringLength(stop))) {
      scan_length++;
      break;
    }

    scan_length++;
  }

  OzoneString string = ((OzoneString) {
      .vector = ((OzoneVectorChar) {
          .elements = ozoneAllocatorReserveMany(allocator, char, scan_length),
          .length = scan_length,
          .capacity = scan_length,
          .capacity_increment = scan_length,
      }),
      .encoding = encoding,
  });

  memcpy(ozoneStringBuffer(&string), buffer, ozoneStringLength(&string) + 1);
  ozoneStringBuffer(&string)[ozoneStringLength(&string)] = '\0';

  return string;
}
