#include "ozone_allocator.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "ozone_log.h"

OzoneAllocatorT *ozoneAllocatorCreate(size_t size)
{
  OzoneAllocatorT *allocator = (OzoneAllocatorT *)malloc(size + sizeof(OzoneAllocatorT));
  ozoneLogTrace("Allocated %ld bytes with malloc, %ld usable as allocation space", size + sizeof(OzoneAllocatorT), size);
  if (!allocator)
    return NULL;

  *allocator = (OzoneAllocatorT){
      .cursor = ozoneAllocatorGetRegionStart(allocator),
      .end = ozoneAllocatorGetRegionStart(allocator) + (uintptr_t)size,
      .previous = NULL,
      .next = NULL};

  return allocator;
}

void ozoneAllocatorDelete(OzoneAllocatorT *allocator)
{
  if (!allocator)
    return;

  OzoneAllocatorT *allocator_iterator = allocator;
  OzoneAllocatorT *next = NULL;
  do
  {
    next = allocator_iterator->next;
    free(allocator_iterator);
  } while ((allocator_iterator = next));
}

size_t ozoneAllocatorGetTotalCapacity(OzoneAllocatorT *allocator)
{
  size_t capacity = 0;
  if (!allocator)
    return capacity;

  OzoneAllocatorT *allocator_iterator = allocator;
  do
  {
    capacity += ozoneAllocatorGetRegionCapacity(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));

  return capacity;
}

size_t ozoneAllocatorGetTotalFree(OzoneAllocatorT *allocator)
{
  size_t free = 0;
  if (!allocator)
    return free;

  OzoneAllocatorT *allocator_iterator = allocator;
  do
  {
    free += ozoneAllocatorGetRegionFree(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));

  return free;
}

uintptr_t ozoneAllocatorReserveBytes(OzoneAllocatorT *allocator, size_t size)
{
  if (!allocator)
    return (uintptr_t)NULL;

  OzoneAllocatorT *allocator_iterator = allocator;
  do
  {
    if (size <= ozoneAllocatorGetRegionFree(allocator_iterator))
    {
      uintptr_t cursor = allocator_iterator->cursor;
      allocator_iterator->cursor += (uintptr_t)size;
      ozoneLogTrace("Reserved %ld bytes among previously allocated bytes", size);
      return cursor;
    }
  } while (allocator_iterator->next && (allocator_iterator = allocator_iterator->next));

  OzoneAllocatorT *new_region = ozoneAllocatorCreate(fmax(size, ozoneAllocatorGetRegionCapacity(allocator)));

  new_region->previous = allocator_iterator;
  new_region->cursor += size;
  allocator_iterator->next = new_region;

  return ozoneAllocatorGetRegionStart(new_region);
}

void ozoneAllocatorClear(OzoneAllocatorT *allocator)
{
  if (!allocator)
    return;

  ozoneLogTrace("Clearing %ld bytes", ozoneAllocatorGetTotalCapacity(allocator));
  OzoneAllocatorT *allocator_iterator = allocator;
  do
  {
    memset((void *)ozoneAllocatorGetRegionStart(allocator_iterator), 0, ozoneAllocatorGetRegionCapacity(allocator_iterator));
    allocator_iterator->cursor = ozoneAllocatorGetRegionStart(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));
}
