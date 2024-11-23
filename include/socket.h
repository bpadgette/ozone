#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "allocator.h"
#include "array.h"

typedef OZArrayT OZSocketRequestT;
#define ozSocketRequestCreate(alloc, size) (OZSocketRequestT *)ozArrayCreate(alloc, size)

typedef OZArrayT OZSocketResponseT;
#define ozSocketResponseCreate(alloc, size) (OZSocketResponseT *)ozArrayCreate(alloc, size)

typedef OZSocketResponseT *(OZSocketHandlerT)(OZAllocatorT *, OZSocketRequestT *);

int ozServeSocket(OZSocketHandlerT *);

#endif
