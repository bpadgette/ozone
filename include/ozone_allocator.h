#ifndef OZONE_ALLOCATOR_H
#define OZONE_ALLOCATOR_H
#include <stddef.h>
#include <stdint.h>

typedef struct OzoneAllocatorStruct {
  uintptr_t cursor;
  uintptr_t end;
  struct OzoneAllocatorStruct* next;
} OzoneAllocator;

OzoneAllocator* ozoneAllocatorCreate(size_t initial_allocation_size);
void ozoneAllocatorDelete(OzoneAllocator* _allocator_);

#define ozoneAllocatorGetRegionStart(_allocator_) (sizeof(OzoneAllocator) + (uintptr_t)_allocator_)
#define ozoneAllocatorGetRegionCapacity(_allocator_)                                                                   \
  (size_t)((_allocator_)->end - ozoneAllocatorGetRegionStart(_allocator_))
size_t ozoneAllocatorGetTotalCapacity(OzoneAllocator* _allocator_);

#define ozoneAllocatorGetRegionFree(_allocator_) (size_t)((_allocator_)->end - (_allocator_)->cursor)
size_t ozoneAllocatorGetTotalFree(OzoneAllocator* _allocator_);

void ozoneAllocatorClear(OzoneAllocator* _allocator_);

uintptr_t ozoneAllocatorReserveBytes(OzoneAllocator* _allocator_, size_t size);
#define ozoneAllocatorReserveOne(_allocator_, _type_)                                                                  \
  (_type_*)ozoneAllocatorReserveBytes(_allocator_, 1 * sizeof(_type_))
#define ozoneAllocatorReserveMany(_allocator_, _type_, _count_)                                                        \
  (_type_*)ozoneAllocatorReserveBytes(_allocator_, (_count_) * sizeof(_type_))

int ozoneAllocatorGrow(OzoneAllocator* allocator, uintptr_t address, size_t current_size, size_t new_size);

#endif
