#ifndef OZONE_VECTOR_H
#define OZONE_VECTOR_H

#include "ozone_allocator.h"
#include <string.h>

#define OZONE_VECTOR_DECLARE_API(_type_)                                                                               \
  typedef struct _type_##VectorStruct {                                                                                \
    _type_* elements;                                                                                                  \
    size_t length;                                                                                                     \
    size_t capacity;                                                                                                   \
    size_t capacity_increment;                                                                                         \
  } _type_##Vector;                                                                                                    \
  void ozoneVectorPush##_type_(OzoneAllocator* allocator, _type_##Vector* vector, _type_ element);

#define OZONE_VECTOR_IMPLEMENT_API(_type_)                                                                             \
  void ozoneVectorPush##_type_(OzoneAllocator* allocator, _type_##Vector* vector, _type_ element) {                    \
    if (vector->length < vector->capacity) {                                                                           \
      vector->elements[vector->length++] = element;                                                                    \
      return;                                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
    if (vector->capacity_increment < 1) {                                                                              \
      vector->capacity_increment = 1;                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    _type_* old = vector->elements;                                                                                    \
    vector->capacity = vector->capacity + vector->capacity_increment;                                                  \
    vector->elements = ozoneAllocatorReserveMany(allocator, _type_, vector->capacity);                                 \
    memcpy(vector->elements, old, sizeof(_type_) * vector->length);                                                    \
    memset(                                                                                                            \
        vector->elements + sizeof(_type_) * vector->length, 0, sizeof(_type_) * (vector->capacity - vector->length));  \
    vector->elements[vector->length] = element;                                                                        \
    vector->length++;                                                                                                  \
  }

#define ozoneVector(_allocator_, _type_, _capacity_)                                                                   \
  ((_type_##Vector) {                                                                                                  \
      .elements = ozoneAllocatorReserveMany(_allocator_, _type_, _capacity_),                                          \
      .length = 0,                                                                                                     \
      .capacity = _capacity_,                                                                                          \
      .capacity_increment = _capacity_,                                                                                \
  })

#define ozoneVectorLength(_vector_) ((_vector_)->length)
#define ozoneVectorBegin(_vector_) (&(_vector_)->elements[0])
#define ozoneVectorEnd(_vector_) (ozoneVectorBegin(_vector_) + ozoneVectorLength(_vector_))
#define ozoneVectorForEach(_iterator_, _vector_)                                                                       \
  for (_iterator_ = ozoneVectorBegin(_vector_); _iterator_ < ozoneVectorEnd(_vector_); _iterator_++)

#endif
