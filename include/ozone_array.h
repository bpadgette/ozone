#ifndef OZONE_ARRAY_H
#define OZONE_ARRAY_H
#include "ozone_allocator.h"
#include <stdint.h>
#include <string.h>

/**
 * For each OZONE_DEFINE_ARRAY_TYPE(type, name) in OZONE_ARRAY_H there must be:
 *   - OZONE_DEFINE_ARRAY_CREATE(type, name) in the implementation
 */
#define OZONE_DEFINE_ARRAY_TYPE(type, name)                                                                            \
  typedef struct Ozone##name##Array {                                                                                  \
    type* data;                                                                                                        \
    size_t length;                                                                                                     \
  } Ozone##name##ArrayT;                                                                                               \
  Ozone##name##ArrayT* ozone##name##ArrayCreate(OzoneAllocatorT* allocator, size_t size)

#define OZONE_DEFINE_ARRAY_CREATE(type, name)                                                                          \
  Ozone##name##ArrayT* ozone##name##ArrayCreate(OzoneAllocatorT* allocator, size_t size)                               \
  {                                                                                                                    \
    Ozone##name##ArrayT* array = (Ozone##name##ArrayT*)ozoneAllocatorReserveBytes(                                     \
        allocator, sizeof(Ozone##name##ArrayT) + size * sizeof(type));                                                 \
    array->data = (type*)(array + sizeof(Ozone##name##ArrayT));                                                        \
    array->length = size;                                                                                              \
    return array;                                                                                                      \
  }

OZONE_DEFINE_ARRAY_TYPE(char, Char);

#define ozoneCharArray(string) ((OzoneCharArrayT) { .data = string, .length = sizeof(string) })

#endif
