#ifndef OZONE_VECTOR_H
#define OZONE_VECTOR_H

#include "ozone_allocator.h"
#include <string.h>

#define OZONE_VECTOR_DECLARE_API(_type_)                                                                               \
  typedef struct _type_##VectorStruct {                                                                                \
    _type_* elements;                                                                                                  \
    _type_* elements_block_begin;                                                                                      \
    size_t length;                                                                                                     \
    size_t capacity;                                                                                                   \
  } _type_##Vector;                                                                                                    \
  void _type_##VectorClear(_type_##Vector* vector);                                                                    \
  void _type_##VectorPop(_type_##Vector* vector, _type_* element);                                                     \
  void _type_##VectorShift(_type_##Vector* vector, _type_* element);                                                   \
  void _type_##VectorPush(OzoneAllocator* allocator, _type_##Vector* vector, _type_* element);

#define ozoneVectorFromArray(_type_, _array_)                                                                          \
  ((_type_##Vector) {                                                                                                  \
      .elements = (_type_*)_array_,                                                                                    \
      .length = sizeof(_array_) / sizeof(_type_),                                                                      \
      .capacity = sizeof(_array_) / sizeof(_type_),                                                                    \
  })

#define ozoneVector(_type_, ...) ozoneVectorFromArray(_type_, ((_type_[]) { __VA_ARGS__ }))

#define ozoneVectorAllocate(_allocator_, _type_, _capacity_)                                                           \
  ((_type_##Vector) {                                                                                                  \
      .elements = ozoneAllocatorReserveMany(_allocator_, _type_, (_capacity_)),                                        \
      .length = 0,                                                                                                     \
      .capacity = _capacity_,                                                                                          \
  })

#define ozoneVectorAt(_vector_, _index_) ((_vector_)->elements[_index_])
#define ozoneVectorBegin(_vector_) ((_vector_)->elements)
#define ozoneVectorEnd(_vector_) (ozoneVectorBegin(_vector_) + ((_vector_)->length))
#define ozoneVectorLast(_vector_) (ozoneVectorEnd(_vector_) - 1)
#define ozoneVectorForEach(_iterator_, _vector_)                                                                       \
  for (_iterator_ = ozoneVectorBegin(_vector_); _iterator_ < ozoneVectorEnd(_vector_); _iterator_++)

#define OZONE_VECTOR_IMPLEMENT_API(_type_)                                                                             \
  void _type_##VectorClear(_type_##Vector* vector) {                                                                   \
    if (!vector->elements_block_begin)                                                                                 \
      vector->elements_block_begin = vector->elements;                                                                 \
                                                                                                                       \
    vector->capacity += vector->elements - vector->elements_block_begin;                                               \
    vector->elements = vector->elements_block_begin;                                                                   \
    vector->length = 0;                                                                                                \
    memset(vector->elements, 0, sizeof(_type_) * (vector->capacity));                                                  \
  }                                                                                                                    \
  void _type_##VectorPop(_type_##Vector* vector, _type_* element) {                                                    \
    if (!vector->elements_block_begin)                                                                                 \
      vector->elements_block_begin = vector->elements;                                                                 \
                                                                                                                       \
    if (!vector->length)                                                                                               \
      return;                                                                                                          \
                                                                                                                       \
    if (element)                                                                                                       \
      memcpy(element, &vector->elements[vector->length - 1], sizeof(_type_));                                          \
                                                                                                                       \
    memset(&vector->elements[--vector->length], 0, sizeof(_type_));                                                    \
  }                                                                                                                    \
  void _type_##VectorShift(_type_##Vector* vector, _type_* element) {                                                  \
    if (!vector->elements_block_begin)                                                                                 \
      vector->elements_block_begin = vector->elements;                                                                 \
                                                                                                                       \
    if (!vector->length)                                                                                               \
      return;                                                                                                          \
                                                                                                                       \
    if (element)                                                                                                       \
      memcpy(element, &vector->elements[0], sizeof(_type_));                                                           \
                                                                                                                       \
    memset(&vector->elements[0], 0, sizeof(_type_));                                                                   \
    vector->elements += 1;                                                                                             \
    vector->capacity--;                                                                                                \
    vector->length--;                                                                                                  \
  }                                                                                                                    \
  void _type_##VectorPush(OzoneAllocator* allocator, _type_##Vector* vector, _type_* element) {                        \
    if (!vector->elements_block_begin)                                                                                 \
      vector->elements_block_begin = vector->elements;                                                                 \
                                                                                                                       \
    if (vector->length >= vector->capacity && vector->elements_block_begin < vector->elements) {                       \
      size_t reclaimable_elements = vector->elements - vector->elements_block_begin;                                   \
      vector->capacity += reclaimable_elements;                                                                        \
      memcpy(vector->elements_block_begin, vector->elements, sizeof(_type_) * vector->length);                         \
      vector->elements = vector->elements_block_begin;                                                                 \
      memset(&vector->elements[vector->length], 0, (vector->capacity - vector->length) * sizeof(_type_));              \
    } else if (vector->length >= vector->capacity) {                                                                   \
      size_t current_capacity = vector->capacity;                                                                      \
      vector->capacity *= 2;                                                                                           \
      if (vector->capacity < 1)                                                                                        \
        vector->capacity = 1;                                                                                          \
                                                                                                                       \
      if (ozoneAllocatorGrow(                                                                                          \
              allocator,                                                                                               \
              (uintptr_t)vector->elements,                                                                             \
              current_capacity * sizeof(_type_),                                                                       \
              vector->capacity * sizeof(_type_))                                                                       \
          == -1) {                                                                                                     \
        _type_* old = vector->elements;                                                                                \
        vector->elements = ozoneAllocatorReserveMany(allocator, _type_, vector->capacity);                             \
        vector->elements_block_begin = vector->elements;                                                               \
        if (old && vector->length)                                                                                     \
          memcpy(vector->elements, old, sizeof(_type_) * vector->length);                                              \
      }                                                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    vector->elements[vector->length++] = *element;                                                                     \
  }

typedef size_t SizeT;
OZONE_VECTOR_DECLARE_API(SizeT)

#endif
