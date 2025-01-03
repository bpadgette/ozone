#include "ozone_string.h"

#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(OzoneStringT)

int ozoneStringCompare(const OzoneStringT* left, const OzoneStringT* right) {
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

  if (left->length < right->length)
    return -1;
  if (left->length > right->length)
    return 1;

  return memcmp(left->buffer, right->buffer, left->length);
}

OzoneStringT* ozoneStringScanBuffer(OzoneAllocatorT* allocator, char* buffer, size_t buffer_size,
    const OzoneStringT* stop, OzoneStringEncodingT encoding) {
  size_t scan_length = 0;
  while (scan_length < buffer_size) {
    if (stop && (buffer_size - scan_length) >= stop->length
        && !memcmp(buffer + scan_length, stop->buffer, stop->length - 1)) {
      scan_length++;
      break;
    }

    scan_length++;
  }

  OzoneStringT* string = ozoneAllocatorReserveOne(allocator, OzoneStringT);
  string->buffer = ozoneAllocatorReserveMany(allocator, char, scan_length);
  string->length = scan_length;
  string->encoding = encoding;

  memcpy(string->buffer, buffer, string->length);
  string->buffer[string->length - 1] = '\0';

  return string;
}
