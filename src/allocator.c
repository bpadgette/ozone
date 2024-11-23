
#include <stdlib.h>
#include <string.h>
#include "allocator.h"
#include "log.h"

OZAllocatorT *ozAllocatorCreate(size_t size)
{
  OZAllocatorT *allocator = (OZAllocatorT *)malloc(size + sizeof(OZAllocatorT));
  ozLogTrace("Allocated %ld bytes with malloc, %ld usable as allocation space", size + sizeof(OZAllocatorT), size);
  if (!allocator)
    return NULL;

  *allocator = (OZAllocatorT){
      .cursor = ozAllocatorGetStart(allocator),
      .end = ozAllocatorGetStart(allocator) + (uintptr_t)size,
      .previous = NULL,
      .next = NULL};

  return allocator;
}

void ozAllocatorDelete(OZAllocatorT *allocator)
{
  if (!allocator)
    return;

  OZAllocatorT *allocator_iterator = allocator;
  OZAllocatorT *next = NULL;
  do
  {
    next = allocator_iterator->next;
    free(allocator_iterator);
  } while ((allocator_iterator = next));
}

size_t ozAllocatorGetTotalCapacity(OZAllocatorT *allocator)
{
  size_t capacity = 0;
  if (!allocator)
    return capacity;

  OZAllocatorT *allocator_iterator = allocator;
  do
  {
    capacity += ozAllocatorGetRegionCapacity(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));

  return capacity;
}

size_t ozAllocatorGetTotalFree(OZAllocatorT *allocator)
{
  size_t free = 0;
  if (!allocator)
    return free;

  OZAllocatorT *allocator_iterator = allocator;
  do
  {
    free += ozAllocatorGetRegionFree(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));

  return free;
}

uintptr_t ozAllocatorReserveBytes(OZAllocatorT *allocator, size_t size)
{
  if (!allocator)
    return (uintptr_t)NULL;

  OZAllocatorT *allocator_iterator = allocator;
  do
  {
    if (size <= ozAllocatorGetRegionFree(allocator_iterator))
    {
      uintptr_t cursor = allocator_iterator->cursor;
      allocator_iterator->cursor += (uintptr_t)size;
      ozLogTrace("Reserved %ld bytes among previously allocated bytes", size);
      return cursor;
    }
  } while ((allocator_iterator = allocator_iterator->next));

  allocator_iterator->next = ozAllocatorCreate(size > ozAllocatorGetRegionCapacity(allocator)
                                                   ? size
                                                   : ozAllocatorGetRegionCapacity(allocator));

  allocator_iterator->next->previous = allocator_iterator;

  return allocator_iterator->next->cursor;
}

void ozAllocatorClear(OZAllocatorT *allocator)
{
  if (!allocator)
    return;

  ozLogTrace("Clearing %ld bytes", ozAllocatorGetTotalCapacity(allocator));
  OZAllocatorT *allocator_iterator = allocator;
  do
  {
    memset((void *)ozAllocatorGetStart(allocator_iterator), 0, ozAllocatorGetRegionCapacity(allocator_iterator));
    allocator_iterator->cursor = ozAllocatorGetStart(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));
}
