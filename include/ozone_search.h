#ifndef OZONE_SEARCH
#define OZONE_SEARCH

long ozoneSearchBinaryInteger(void* array, long array_length, unsigned short integer_size, long search);

#define ozoneVectorSearchBinary(_type_, _vector_, _search_)                                                            \
  ozoneSearchBinaryInteger((_vector_)->elements, (_vector_)->length, sizeof(_type_), (long)(_search_))

#endif
