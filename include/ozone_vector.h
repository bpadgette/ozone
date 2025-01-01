#ifndef OZONE_VECTOR_H
#define OZONE_VECTOR_H

#include "ozone_allocator.h"
#include <string.h>

#define OZONE_VECTOR_DECLARE_API(_type_, _function_prefix_)                                                            \
  typedef struct _type_##Vector {                                                                                      \
    _type_* elements;                                                                                                  \
    size_t length;                                                                                                     \
    size_t capacity;                                                                                                   \
    size_t capacity_increment;                                                                                         \
  } _type_##Vector##T;                                                                                                 \
  _type_##Vector##T* _function_prefix_##VectorCreate(OzoneAllocatorT* allocator, size_t capacity);                     \
  void _function_prefix_##VectorPush(OzoneAllocatorT* allocator, _type_##Vector##T* vector, _type_ element);

#define OZONE_VECTOR_DEFINE_API(_type_, _function_prefix_)                                                             \
  _type_##Vector##T* _function_prefix_##VectorCreate(OzoneAllocatorT* allocator, size_t capacity) {                    \
    _type_##Vector##T* vector = ozoneAllocatorReserveOne(allocator, _type_##Vector##T);                                \
    *vector = (_type_##Vector##T) {                                                                                    \
      .elements = ozoneAllocatorReserveMany(allocator, _type_, capacity),                                              \
      .length = 0,                                                                                                     \
      .capacity = capacity,                                                                                            \
      .capacity_increment = capacity,                                                                                  \
    };                                                                                                                 \
                                                                                                                       \
    return vector;                                                                                                     \
  }                                                                                                                    \
                                                                                                                       \
  void _function_prefix_##VectorPush(OzoneAllocatorT* allocator, _type_##Vector##T* vector, _type_ element) {          \
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

#endif
