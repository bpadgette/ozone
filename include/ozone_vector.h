#ifndef OZONE_VECTOR_H
#define OZONE_VECTOR_H

#include "ozone_allocator.h"
#include <string.h>

#define OZONE_VECTOR_DECLARE_API(_type_)                                                                               \
  typedef struct Ozone##_type_##Vector {                                                                               \
    Ozone##_type_* elements;                                                                                           \
    size_t length;                                                                                                     \
    size_t capacity;                                                                                                   \
    size_t capacity_increment;                                                                                         \
  } Ozone##_type_##VectorT;                                                                                            \
  Ozone##_type_##VectorT* ozone##_type_##VectorCreate(OzoneAllocatorT* allocator, size_t capacity);                    \
  void ozone##_type_##VectorPush(OzoneAllocatorT* allocator, Ozone##_type_##VectorT* vector, Ozone##_type_ element);

#define OZONE_VECTOR_IMPLEMENT_API(_type_)                                                                             \
  Ozone##_type_##VectorT* ozone##_type_##VectorCreate(OzoneAllocatorT* allocator, size_t capacity) {                   \
    Ozone##_type_##VectorT* vector = ozoneAllocatorReserveOne(allocator, Ozone##_type_##VectorT);                      \
    *vector = (Ozone##_type_##VectorT) {                                                                               \
      .elements = ozoneAllocatorReserveMany(allocator, Ozone##_type_, capacity),                                       \
      .length = 0,                                                                                                     \
      .capacity = capacity,                                                                                            \
      .capacity_increment = capacity,                                                                                  \
    };                                                                                                                 \
                                                                                                                       \
    return vector;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  void ozone##_type_##VectorPush(OzoneAllocatorT* allocator, Ozone##_type_##VectorT* vector, Ozone##_type_ element) {  \
    if (vector->length < vector->capacity) {                                                                           \
      vector->elements[vector->length++] = element;                                                                    \
      return;                                                                                                          \
    }                                                                                                                  \
                                                                                                                       \
    if (vector->capacity_increment < 1) {                                                                              \
      vector->capacity_increment = 1;                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Ozone##_type_* old = vector->elements;                                                                             \
    vector->capacity = vector->capacity + vector->capacity_increment;                                                  \
    vector->elements = ozoneAllocatorReserveMany(allocator, Ozone##_type_, vector->capacity);                          \
    memcpy(vector->elements, old, sizeof(Ozone##_type_) * vector->length);                                             \
    vector->elements[vector->length++] = element;                                                                      \
  }

#endif
