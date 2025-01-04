#ifndef OZONE_STRING_H
#define OZONE_STRING_H

#include "ozone_allocator.h"
#include "ozone_vector.h"

OZONE_VECTOR_DECLARE_API(char)
typedef charVectorT OzoneVectorCharT;

#define ozoneVectorChar(_chars_, _encoding_)                                                                           \
  ((OzoneVectorCharT) {                                                                                                \
      .elements = _chars_,                                                                                             \
      .length = sizeof(_chars_),                                                                                       \
      .capacity = sizeof(_chars_),                                                                                     \
      .capacity_increment = sizeof(_chars_),                                                                           \
  })

typedef enum OzoneStringEncoding {
  OZONE_STRING_ENCODING_UNKNOWN,
  OZONE_STRING_ENCODING_ISO_8859_1,
} OzoneStringEncodingT;

typedef struct OzoneString {
  OzoneVectorCharT vector;
  OzoneStringEncodingT encoding;
} OzoneStringT;

OZONE_VECTOR_DECLARE_API(OzoneStringT)

#define ozoneStringEncoded(_chars_, _encoding_)                                                                        \
  ((OzoneStringT) {                                                                                                    \
      .vector = ((OzoneVectorCharT) {                                                                                  \
          .elements = _chars_,                                                                                         \
          .length = sizeof(_chars_),                                                                                   \
          .capacity = sizeof(_chars_),                                                                                 \
          .capacity_increment = sizeof(_chars_),                                                                       \
      }),                                                                                                              \
      .encoding = _encoding_,                                                                                          \
  })

#define ozoneString(_chars_) ozoneStringEncoded(_chars_, OZONE_STRING_ENCODING_ISO_8859_1)
#define ozoneStringLength(_string_) ((_string_)->vector.length)
#define ozoneStringBuffer(_string_) ((_string_)->vector.elements)
#define ozoneStringBufferAt(_string_, _index_) ((_string_)->vector.elements[_index_])
#define ozoneStringAppendChar(_allocator_, _string_, _char_)                                                           \
  do {                                                                                                                 \
    ozoneVectorPushchar(_allocator_, &(_string_)->vector, '\0');                                                       \
    (_string_)->vector.elements[(_string_)->vector.length - 2] = _char_;                                               \
  } while (0)

OzoneStringT ozoneStringCopy(OzoneAllocatorT* allocator, const OzoneStringT* original);

/**
 * \returns 0 if equal, negative if left less than right, positive if left greater than right
 */
int ozoneStringCompare(const OzoneStringT* left, const OzoneStringT* right);

/**
 * \returns  OzoneStringT, not exceeding the buffer_size and not including the first occurrence of *stop
 * and beyond. If stop is NULL, then it is ignored and this function will scan until buffer_size is reached.
 */
OzoneStringT ozoneStringScanBuffer(OzoneAllocatorT* allocator, char* buffer, size_t buffer_size,
    const OzoneStringT* stop, OzoneStringEncodingT encoding);

#endif
