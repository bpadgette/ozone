#ifndef OZONE_SPARSE_VECTOR
#define OZONE_SPARSE_VECTOR

#include "ozone_search.h"
#include "ozone_vector.h"

#define OZONE_SPARSE_VECTOR_DECLARE_API(_type_)                                                                        \
  typedef struct _type_##SparseVectorStruct {                                                                          \
    SizeTVector positions;                                                                                             \
    _type_##Vector compressed;                                                                                         \
  } _type_##SparseVector;                                                                                              \
  void _type_##SparseVectorInsert(                                                                                     \
      OzoneAllocator* allocator, _type_##SparseVector* sparse_vector, size_t position, _type_* element);               \
  void _type_##SparseVectorDelete(_type_##SparseVector* sparse_vector, size_t position);                               \
  _type_* _type_##SparseVectorFind(_type_##SparseVector* sparse_vector, size_t position);

#define OZONE_SPARSE_VECTOR_IMPLEMENT_API(_type_)                                                                      \
  void _type_##SparseVectorInsert(                                                                                     \
      OzoneAllocator* allocator, _type_##SparseVector* sparse_vector, size_t position, _type_* element) {              \
    size_t compressed_position = ozoneVectorSearchBinary(_type_, &sparse_vector->positions, position);                 \
    if (compressed_position != -1) {                                                                                   \
      sparse_vector->compressed[compressed_position] = *element;                                                       \
      return;                                                                                                          \
    }                                                                                                                  \
  }                                                                                                                    \
  void _type_##SparseVectorDelete(_type_##SparseVector* sparse_vector, size_t position) { }                            \
  _type_* _type_##SparseVectorFind(_type_##SparseVector* sparse_vector, size_t position) {                             \
    size_t compressed_position = ozoneVectorSearchBinary(_type_, &sparse_vector->positions, position);                 \
    if (compressed_position == -1)                                                                                     \
      return NULL;                                                                                                     \
                                                                                                                       \
    return &ozoneVectorAt(&sparse_vector->compressed, compressed_position);                                            \
  }

#endif
