#include "ozone_allocator.h"

#include "ozone_log.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

OzoneAllocator* ozoneAllocatorCreate(size_t size) {
  OzoneAllocator* allocator = (OzoneAllocator*)malloc(size + sizeof(OzoneAllocator));
  ozoneLogDebug("Allocated %ld bytes with malloc, %ld usable as allocation space", size + sizeof(OzoneAllocator), size);
  if (!allocator)
    return NULL;

  *allocator = (OzoneAllocator) { .cursor = ozoneAllocatorGetRegionStart(allocator),
                                  .end = ozoneAllocatorGetRegionStart(allocator) + (uintptr_t)size,
                                  .next = NULL };

  return allocator;
}

void ozoneAllocatorDelete(OzoneAllocator* allocator) {
  if (!allocator)
    return;

  OzoneAllocator* allocator_iterator = allocator;
  OzoneAllocator* next = NULL;
  do {
    next = allocator_iterator->next;
    free(allocator_iterator);
  } while ((allocator_iterator = next));
}

size_t ozoneAllocatorGetTotalCapacity(OzoneAllocator* allocator) {
  size_t capacity = 0;
  if (!allocator)
    return capacity;

  OzoneAllocator* allocator_iterator = allocator;
  do {
    capacity += ozoneAllocatorGetRegionCapacity(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));

  return capacity;
}

size_t ozoneAllocatorGetTotalFree(OzoneAllocator* allocator) {
  size_t free = 0;
  if (!allocator)
    return free;

  OzoneAllocator* allocator_iterator = allocator;
  do {
    free += ozoneAllocatorGetRegionFree(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));

  return free;
}

uintptr_t ozoneAllocatorReserveBytes(OzoneAllocator* allocator, size_t size) {
  if (!allocator)
    return (uintptr_t)NULL;

  OzoneAllocator* allocator_iterator = allocator;
  do {
    if (size <= ozoneAllocatorGetRegionFree(allocator_iterator)) {
      uintptr_t cursor = allocator_iterator->cursor;
      allocator_iterator->cursor += (uintptr_t)size;
      ozoneLogTrace("Reserved %ld bytes among previously allocated bytes", size);
      return cursor;
    }
  } while (allocator_iterator->next && (allocator_iterator = allocator_iterator->next));

  OzoneAllocator* new_region = ozoneAllocatorCreate(fmax(size, ozoneAllocatorGetRegionCapacity(allocator)));

  new_region->cursor += size;
  allocator_iterator->next = new_region;

  return ozoneAllocatorGetRegionStart(new_region);
}

void ozoneAllocatorClear(OzoneAllocator* allocator) {
  if (!allocator)
    return;

  ozoneLogTrace("Clearing %ld bytes", ozoneAllocatorGetTotalCapacity(allocator));
  OzoneAllocator* allocator_iterator = allocator;
  do {
    memset(
        (void*)ozoneAllocatorGetRegionStart(allocator_iterator),
        0,
        ozoneAllocatorGetRegionCapacity(allocator_iterator));
    allocator_iterator->cursor = ozoneAllocatorGetRegionStart(allocator_iterator);
  } while ((allocator_iterator = allocator_iterator->next));
}
