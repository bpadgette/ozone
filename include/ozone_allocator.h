#ifndef OZONE_ALLOCATOR_H
#define OZONE_ALLOCATOR_H
#include <stdint.h>
#include <stddef.h>

typedef struct OzoneAllocator
{
  uintptr_t cursor;
  uintptr_t end;
  struct OzoneAllocator *previous;
  struct OzoneAllocator *next;
} OzoneAllocatorT;

OzoneAllocatorT *ozoneAllocatorCreate(size_t initial_allocation_size);
void ozoneAllocatorDelete(OzoneAllocatorT *allocator);

#define ozoneAllocatorGetRegionStart(allocator) (sizeof(OzoneAllocatorT) + (uintptr_t)allocator)
#define ozoneAllocatorGetRegionCapacity(allocator) (size_t)(allocator->end - ozoneAllocatorGetRegionStart(allocator))
size_t ozoneAllocatorGetTotalCapacity(OzoneAllocatorT *allocator);

#define ozoneAllocatorGetRegionFree(allocator) (size_t)(allocator->end - allocator->cursor)
size_t ozoneAllocatorGetTotalFree(OzoneAllocatorT *allocator);

void ozoneAllocatorClear(OzoneAllocatorT *allocator);

uintptr_t ozoneAllocatorReserveBytes(OzoneAllocatorT *allocator, size_t size);
#define ozoneAllocatorReserveOne(allocator, type) (type *)ozoneAllocatorReserveBytes(allocator, 1 * sizeof(type))
#define ozoneAllocatorReserveMany(allocator, type, count) (type *)ozoneAllocatorReserveBytes(allocator, count * sizeof(type))

#endif
