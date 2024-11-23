#ifndef OZONE_SOCKET_H
#define OZONE_SOCKET_H
#include "allocator.h"
#include "array.h"

typedef OZCharArrayT OZSocketRequestT;
#define ozSocketRequestCreate(alloc, size) (OZSocketRequestT *)ozCharArrayCreate(alloc, size)

typedef OZCharArrayT OZSocketResponseT;
#define ozSocketResponseCreate(alloc, size) (OZSocketResponseT *)ozCharArrayCreate(alloc, size)

typedef OZSocketResponseT *(OZSocketHandlerT)(OZAllocatorT *, OZSocketRequestT *);

int ozServeSocket(OZSocketHandlerT *);

#endif
