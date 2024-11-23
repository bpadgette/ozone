#ifndef OZONE_ALLOCATOR_H
#define OZONE_ALLOCATOR_H
#include <stdint.h>
#include <stddef.h>

typedef struct OZAllocator
{
  uintptr_t cursor;
  uintptr_t end;
  struct OZAllocator *previous;
  struct OZAllocator *next;
} OZAllocatorT;

OZAllocatorT *ozAllocatorCreate(size_t initial_allocation_size);
void ozAllocatorDelete(OZAllocatorT *allocator);

#define ozAllocatorGetStart(allocator) (sizeof(OZAllocatorT) + (uintptr_t)allocator)
#define ozAllocatorGetRegionCapacity(allocator) (size_t)(allocator->end - ozAllocatorGetStart(allocator))
size_t ozAllocatorGetTotalCapacity(OZAllocatorT *allocator);

#define ozAllocatorGetRegionFree(allocator) (size_t)(allocator->end - allocator->cursor)
size_t ozAllocatorGetTotalFree(OZAllocatorT *allocator);

void ozAllocatorClear(OZAllocatorT *allocator);

uintptr_t ozAllocatorReserveBytes(OZAllocatorT *allocator, size_t size);
#define ozAllocatorReserveOne(allocator, type) (type *)ozAllocatorReserveBytes(allocator, 1 * sizeof(type))
#define ozAllocatorReserveMany(allocator, type, count) (type *)ozAllocatorReserveBytes(allocator, count * sizeof(type))

#endif
