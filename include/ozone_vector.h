#ifndef OZONE_VECTOR_H
#define OZONE_VECTOR_H

#include "ozone_allocator.h"
#include <string.h>

#define OZONE_VECTOR_DECLARE_API(_type_)                                                                               \
  typedef struct _type_##Vector {                                                                                      \
    _type_* elements;                                                                                                  \
    size_t length;                                                                                                     \
    size_t capacity;                                                                                                   \
    size_t capacity_increment;                                                                                         \
  } _type_##VectorT;                                                                                                   \
  void ozoneVectorPush##_type_(OzoneAllocatorT* allocator, _type_##VectorT* vector, _type_ element);

#define OZONE_VECTOR_IMPLEMENT_API(_type_)                                                                             \
  void ozoneVectorPush##_type_(OzoneAllocatorT* allocator, _type_##VectorT* vector, _type_ element) {                  \
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
    vector->elements[vector->length++] = element;                                                                      \
  }

#define ozoneVector(_allocator_, _type_, _capacity_)                                                                   \
  ((_type_##VectorT) {                                                                                                 \
      .elements = ozoneAllocatorReserveMany(_allocator_, _type_, _capacity_),                                          \
      .length = 0,                                                                                                     \
      .capacity = _capacity_,                                                                                          \
      .capacity_increment = _capacity_,                                                                                \
  })
#endif
