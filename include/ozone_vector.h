#ifndef OZONE_VECTOR_H
#define OZONE_VECTOR_H

#include "ozone_allocator.h"
#include <string.h>

#define OZONE_VECTOR_DECLARE_API(_type_)                                                                               \
  typedef struct _type_##VectorStruct {                                                                                \
    _type_* elements;                                                                                                  \
    size_t length;                                                                                                     \
    size_t capacity;                                                                                                   \
  } _type_##Vector;                                                                                                    \
  void ozoneVectorClear##_type_(_type_##Vector* vector);                                                               \
  void ozoneVectorPush##_type_(OzoneAllocator* allocator, _type_##Vector* vector, _type_* element);

#define ozoneVectorFromArray(_type_, _array_)                                                                          \
  ((_type_##Vector) {                                                                                                  \
      .elements = (_type_*)_array_,                                                                                    \
      .length = sizeof(_array_) / sizeof(_type_),                                                                      \
      .capacity = sizeof(_array_) / sizeof(_type_),                                                                    \
  })

#define ozoneVectorFromElements(_type_, ...)                                                                           \
  ((_type_##Vector) {                                                                                                  \
      .elements = ((_type_[]) { __VA_ARGS__ }),                                                                        \
      .length = sizeof(((_type_[]) { __VA_ARGS__ })) / sizeof(_type_),                                                 \
      .capacity = sizeof(((_type_[]) { __VA_ARGS__ })) / sizeof(_type_),                                               \
  })

#define ozoneVectorLength(_vector_) ((_vector_)->length)
#define ozoneVectorAt(_vector_, _index_) ((_vector_)->elements[_index_])
#define ozoneVectorBegin(_vector_) ((_vector_)->elements)
#define ozoneVectorIndex(_vector_, _iterator_) ((_iterator_) - ozoneVectorBegin(_vector_))
#define ozoneVectorEnd(_vector_) (ozoneVectorBegin(_vector_) + ozoneVectorLength(_vector_))
#define ozoneVectorForEach(_iterator_, _vector_)                                                                       \
  for (_iterator_ = ozoneVectorBegin(_vector_); _iterator_ < ozoneVectorEnd(_vector_); _iterator_++)

#define OZONE_VECTOR_IMPLEMENT_API(_type_)                                                                             \
  void ozoneVectorClear##_type_(_type_##Vector* vector) {                                                              \
    memset(vector->elements, 0, sizeof(_type_) * (vector->capacity));                                                  \
    vector->capacity = 0;                                                                                              \
    vector->length = 0;                                                                                                \
  }                                                                                                                    \
  void ozoneVectorPush##_type_(OzoneAllocator* allocator, _type_##Vector* vector, _type_* element) {                   \
    if (vector->length >= vector->capacity) {                                                                          \
      vector->capacity *= 2;                                                                                           \
      if (vector->capacity < 1)                                                                                        \
        vector->capacity = 1;                                                                                          \
                                                                                                                       \
      _type_* old = vector->elements;                                                                                  \
      vector->elements = ozoneAllocatorReserveMany(allocator, _type_, vector->capacity);                               \
      if (old && vector->length)                                                                                       \
        memcpy(vector->elements, old, sizeof(_type_) * vector->length);                                                \
    }                                                                                                                  \
                                                                                                                       \
    vector->elements[vector->length++] = *element;                                                                     \
  }

#endif
