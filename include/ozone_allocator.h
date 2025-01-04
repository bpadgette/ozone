#ifndef OZONE_ALLOCATOR_H
#define OZONE_ALLOCATOR_H
#include <stddef.h>
#include <stdint.h>

typedef struct OzoneAllocatorStruct {
  uintptr_t cursor;
  uintptr_t end;
  struct OzoneAllocatorStruct* previous;
  struct OzoneAllocatorStruct* next;
} OzoneAllocator;

OzoneAllocator* ozoneAllocatorCreate(size_t initial_allocation_size);
void ozoneAllocatorDelete(OzoneAllocator* allocator);

#define ozoneAllocatorGetRegionStart(allocator) (sizeof(OzoneAllocator) + (uintptr_t)allocator)
#define ozoneAllocatorGetRegionCapacity(allocator) (size_t)(allocator->end - ozoneAllocatorGetRegionStart(allocator))
size_t ozoneAllocatorGetTotalCapacity(OzoneAllocator* allocator);

#define ozoneAllocatorGetRegionFree(allocator) (size_t)(allocator->end - allocator->cursor)
size_t ozoneAllocatorGetTotalFree(OzoneAllocator* allocator);

void ozoneAllocatorClear(OzoneAllocator* allocator);

uintptr_t ozoneAllocatorReserveBytes(OzoneAllocator* allocator, size_t size);
#define ozoneAllocatorReserveOne(allocator, type) (type*)ozoneAllocatorReserveBytes(allocator, 1 * sizeof(type))
#define ozoneAllocatorReserveMany(allocator, type, count)                                                              \
  (type*)ozoneAllocatorReserveBytes(allocator, count * sizeof(type))

#endif
