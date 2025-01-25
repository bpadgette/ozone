#include "test.h"

#include "ozone_allocator.h"

void shouldReserveAndClearSingleRegion(void) {
  size_t size = 1024;
  OzoneAllocator* alloc = ozoneAllocatorCreate(size);
  char* allocation = ozoneAllocatorReserveMany(alloc, char, size);

  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalCapacity(alloc) == size, "It allocates capacity equal to size");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionCapacity(alloc) == size, "It allocates capacity to single region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == 0, "It has 0 free bytes in allocator");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionFree(alloc) == 0, "It has 0 free bytes in allocator region");

  for (size_t i = 0; i < size; i++) {
    TEST_ASSERT_MESSAGE(allocation[i] == 0, "It grants access to full zeroed allocation");
  }

  ozoneAllocatorClear(alloc);
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalCapacity(alloc) == size, "It has total capacity equal to size after clear");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == size, "It has total free space equal to size after clear");

  ozoneAllocatorDelete(alloc);
}

void shouldReserveAndClearMultipleRegions(void) {
  size_t size = 1024;
  OzoneAllocator* alloc = ozoneAllocatorCreate(size);

  char* allocation = ozoneAllocatorReserveMany(alloc, char, size);
  char* second_allocation = ozoneAllocatorReserveMany(alloc, char, size);

  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalCapacity(alloc) == size * 2, "It allocates capacity equal to size * 2");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionCapacity(alloc) == size, "It allocates capacity to first region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionCapacity(alloc->next) == size, "It allocates capacity to second region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == 0, "It has 0 free bytes in allocator");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionFree(alloc) == 0, "It has 0 free bytes in first region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionFree(alloc->next) == 0, "It has 0 free bytes in second region");
  TEST_ASSERT_MESSAGE(allocation[size - 1] == 0, "It grants access to full allocation");
  TEST_ASSERT_MESSAGE(second_allocation[size - 1] == 0, "It grants access to full second allocation");

  ozoneAllocatorClear(alloc);
  TEST_ASSERT_MESSAGE(
      ozoneAllocatorGetTotalCapacity(alloc) == size * 2, "It has total capacity equal to size after clear");
  TEST_ASSERT_MESSAGE(
      ozoneAllocatorGetTotalFree(alloc) == size * 2, "It has total free space equal to size after clear");

  ozoneAllocatorDelete(alloc);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldReserveAndClearSingleRegion);
  RUN_TEST(shouldReserveAndClearMultipleRegions);
  return UNITY_END();
}
