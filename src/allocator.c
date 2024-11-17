#include "allocator.h"
#include <stdlib.h>

Allocator *createAllocator(size_t minimum_region_capacity)
{
  Allocator *allocator = malloc(sizeof(Allocator));
  if (!allocator)
  {
    return NULL;
  }

  allocator->minimum_region_capacity = minimum_region_capacity;
  allocator->head_region = NULL;
  allocator->tail_region = NULL;

  return allocator;
}

size_t deleteAllocator(Allocator *allocator)
{
  size_t deallocated = 0;
  AllocatorRegion *cursor = NULL;
  AllocatorRegion *next_cursor = allocator->head_region;
  while ((cursor = next_cursor))
  {
    next_cursor = cursor->next_region;
    deallocated += (sizeof(AllocatorRegion) + cursor->total_capacity);
    free(cursor);
  };

  free(allocator);
  deallocated += sizeof(allocator);

  return deallocated;
}

uintptr_t pushAllocator(Allocator *allocator, size_t size)
{
  if (allocator->tail_region &&
      (allocator->tail_region->total_capacity - allocator->tail_region->used_capacity) >= size)
  {
    uintptr_t content_position = allocator->tail_region->content + allocator->tail_region->used_capacity;
    allocator->tail_region->used_capacity += size;

    return content_position;
  }

  size_t capacity = size > allocator->minimum_region_capacity
                        ? size
                        : allocator->minimum_region_capacity;

  AllocatorRegion *region = malloc(sizeof(AllocatorRegion) + capacity);
  if (!region)
  {
    return (uintptr_t)NULL;
  }

  region->content = (uintptr_t)(region + sizeof(AllocatorRegion));
  region->total_capacity = capacity;
  region->used_capacity = 0;
  region->next_region = NULL;

  if (!allocator->head_region)
  {
    allocator->head_region = region;
  }

  if (allocator->tail_region)
  {
    allocator->tail_region->next_region = region;
  }

  allocator->tail_region = region;

  return region->content;
}
