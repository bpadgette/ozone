#ifndef WEBSERVER_ALLOCATOR_H
#define WEBSERVER_ALLOCATOR_H
#include <stddef.h>
#include <stdint.h>

typedef struct AllocatorRegion
{
  struct AllocatorRegion *next_region;
  size_t used_capacity;
  size_t total_capacity;
  uintptr_t content;
} AllocatorRegion;

typedef struct Allocator
{
  AllocatorRegion *head_region, *tail_region;
  size_t minimum_region_capacity;
} Allocator;

Allocator *createAllocator(size_t minimum_region_capacity);
#define createStrictAllocator() createAllocator(0)
#define create8KBAllocator() createAllocator(8 * 1024)
#define create16KBAllocator() createAllocator(16 * 1024)
#define create32KBAllocator() createAllocator(32 * 1024)

size_t deleteAllocator(Allocator *allocator);

uintptr_t growAllocator(Allocator *allocator, size_t size);
#define pushAllocator(allocator, type, count) (type *)growAllocator(allocator, count * sizeof(type))

#endif
