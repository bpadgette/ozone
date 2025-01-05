#ifndef OZONE_STRING_H
#define OZONE_STRING_H

#include "ozone_allocator.h"
#include "ozone_vector.h"

OZONE_VECTOR_DECLARE_API(char)
typedef charVector OzoneVectorChar;

typedef enum OzoneStringEncoding {
  OZONE_STRING_ENCODING_UNKNOWN,
  OZONE_STRING_ENCODING_ISO_8859_1,
} OzoneStringEncoding;

typedef struct OzoneStringStruct {
  OzoneVectorChar vector;
  OzoneStringEncoding encoding;
} OzoneString;

OZONE_VECTOR_DECLARE_API(OzoneString)

#define ozoneStringEncoded(_chars_, _encoding_)                                                                        \
  ((OzoneString) {                                                                                                     \
      .vector = ((OzoneVectorChar) {                                                                                   \
          .elements = _chars_,                                                                                         \
          .length = sizeof(_chars_),                                                                                   \
          .capacity = sizeof(_chars_),                                                                                 \
      }),                                                                                                              \
      .encoding = _encoding_,                                                                                          \
  })

#define ozoneString(_chars_) ozoneStringEncoded(_chars_, OZONE_STRING_ENCODING_ISO_8859_1)
#define ozoneStringLength(_string_)                                                                                    \
  ((_string_)->vector.length > 0 ? (_string_)->vector.length - 1 : (_string_)->vector.length)
#define ozoneStringBuffer(_string_) ((_string_)->vector.elements)
#define ozoneStringBufferAt(_string_, _index_) ((_string_)->vector.elements[_index_])

void ozoneStringAppend(OzoneAllocator* allocator, OzoneString* string, char byte);
void ozoneStringClear(OzoneString* string);
char ozoneStringPop(OzoneString* string);
OzoneString* ozoneStringCreate(OzoneAllocator* allocator, size_t capacity);
OzoneString ozoneStringCopy(OzoneAllocator* allocator, const OzoneString* original);
OzoneString ozoneStringJoin(OzoneAllocator* allocator, const OzoneStringVector* vector, OzoneStringEncoding encoding);

/**
 * \returns -1 if not found, or the index of the first occurrence of the search string.
 */
int ozoneStringFindFirst(const OzoneString* string, const OzoneString* search);

/**
 * \returns 0 if equal, negative if left less than right, positive if left greater than right
 */
int ozoneStringCompare(const OzoneString* left, const OzoneString* right);

/**
 * \returns  OzoneString, not exceeding the buffer_size and not scanning beyond the first occurrence of *end.
 * If end is NULL then this function will scan until buffer_size is reached.
 */
OzoneString ozoneStringFromBuffer(
    OzoneAllocator* allocator, char* buffer, size_t buffer_size, const OzoneString* end, OzoneStringEncoding encoding);

#endif
