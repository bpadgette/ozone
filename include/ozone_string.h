#ifndef OZONE_STRING_H
#define OZONE_STRING_H

#include "ozone_allocator.h"
#include "ozone_vector.h"

typedef enum OzoneStringEncoding {
  OZONE_STRING_ENCODING_UNKNOWN,
  OZONE_STRING_ENCODING_ISO_8859_1,
} OzoneStringEncodingT;

typedef struct OzoneString {
  char* buffer;
  size_t length;
  OzoneStringEncodingT encoding;
} OzoneStringT;

OZONE_VECTOR_DECLARE_API(OzoneStringT)

#define ozoneString(_chars_, _encoding_)                                                                               \
  ((OzoneStringT) { .buffer = _chars_, .length = sizeof(_chars_), .encoding = _encoding_ })

#define ozoneCharArray(_chars_) ozoneString(_chars_, OZONE_STRING_ENCODING_ISO_8859_1)

/**
 * \returns 0 if equal, negative if left less than right, positive if left greater than right
 */
int ozoneStringCompare(const OzoneStringT* left, const OzoneStringT* right);

/**
 * \returns pointer to new OzoneStringT, not exceeding the buffer_size and not including the first occurrence of *stop
 * and beyond. If stop is NULL, then it is ignored and this function will scan until buffer_size is reached.
 */
OzoneStringT* ozoneStringScanBuffer(OzoneAllocatorT* allocator, char* buffer, size_t buffer_size,
    const OzoneStringT* stop, OzoneStringEncodingT encoding);

#define ozoneCharArrayScanBuffer(_allocator_, _buffer_, _buffer_size_, _stop_)                                         \
  ozoneStringScanBuffer(_allocator_, _buffer_, _buffer_size_, _stop_, OZONE_STRING_ENCODING_ISO_8859_1)

#endif
