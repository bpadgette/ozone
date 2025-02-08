#ifndef OZONE_STRING_H
#define OZONE_STRING_H

#include "ozone_allocator.h"
#include "ozone_vector.h"

typedef char OzoneByte;
OZONE_VECTOR_DECLARE_API(OzoneByte)

typedef struct OzoneStringStruct {
  OzoneByteVector vector;
} OzoneString;

OZONE_VECTOR_DECLARE_API(OzoneString)

#define ozoneStringConstant(_chars_)                                                                                   \
  ((OzoneString) {                                                                                                     \
      .vector = ((OzoneByteVector) {                                                                                   \
          .elements = _chars_,                                                                                         \
          .length = sizeof(_chars_),                                                                                   \
          .capacity = sizeof(_chars_),                                                                                 \
      }),                                                                                                              \
  })

#define ozoneString(_allocator_, _chars_) ozoneStringCopy(_allocator_, &ozoneStringConstant((_chars_)))

#define ozoneStringLength(_string_)                                                                                    \
  (ozoneVectorLength(&(_string_)->vector) > 0 ? ozoneVectorLength(&(_string_)->vector) - 1 : 0)
#define ozoneStringBuffer(_string_) ((_string_)->vector.elements)
#define ozoneStringBufferAt(_string_, _index_) ((_string_)->vector.elements[_index_])
#define ozoneStringBufferEnd(_string_) ((_string_)->vector.elements[ozoneStringLength(_string_) - 1])

void ozoneStringAppend(OzoneAllocator* allocator, OzoneString* string, char byte);
void ozoneStringClear(OzoneString* string);
char ozoneStringPop(OzoneString* string);
OzoneString* ozoneStringCopy(OzoneAllocator* allocator, const OzoneString* original);
void ozoneStringConcatenate(OzoneAllocator* allocator, OzoneString* destination, const OzoneString* source);
void ozoneStringJoin(OzoneAllocator* allocator, OzoneString* destination, const OzoneStringVector* strings);
OzoneString* ozoneStringSlice(OzoneAllocator* allocator, OzoneString* string, size_t begin, size_t end);

/**
 * \returns -1 if not found, or the index of the first occurrence of the search string.
 */
int ozoneStringFindFirst(const OzoneString* string, const OzoneString* search);

/**
 * \returns 0 if equal, negative if left less than right, positive if left greater than right
 */
int ozoneStringCompare(const OzoneString* left, const OzoneString* right);

OzoneString* ozoneStringFromBuffer(OzoneAllocator* allocator, char* buffer, size_t buffer_size);
OzoneString* ozoneStringFromInteger(OzoneAllocator* allocator, long int input);
long int ozoneStringToInteger(const OzoneString* input);

#endif
