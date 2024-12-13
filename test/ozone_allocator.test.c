#include "ozone_allocator.h"
#include "unity.h"

void setUp(void) { }

void tearDown(void) { }

void shouldReserveAndClearSingleRegion(void)
{
  size_t size = 1024;
  OzoneAllocatorT* alloc = ozoneAllocatorCreate(size);
  char* allocation = ozoneAllocatorReserveMany(alloc, char, size);

  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalCapacity(alloc) == size, "allocates capacity equal to size");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionCapacity(alloc) == size, "allocates capacity to single region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == 0, "has 0 free bytes in allocator");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionFree(alloc) == 0, "has 0 free bytes in allocator region");
  TEST_ASSERT_MESSAGE(allocation[size - 1] == 0, "grants access to full allocation");

  ozoneAllocatorClear(alloc);
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalCapacity(alloc) == size, "has total capacity equal to size after clear");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == size, "has total free space equal to size after clear");

  ozoneAllocatorDelete(alloc);
}

void shouldReserveAndClearMultipleRegions(void)
{
  size_t size = 1024;
  OzoneAllocatorT* alloc = ozoneAllocatorCreate(size);

  char* allocation = ozoneAllocatorReserveMany(alloc, char, size);
  char* second_allocation = ozoneAllocatorReserveMany(alloc, char, size);

  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalCapacity(alloc) == size * 2, "allocates capacity equal to size * 2");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionCapacity(alloc) == size, "allocates capacity to first region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionCapacity(alloc->next) == size, "allocates capacity to second region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == 0, "has 0 free bytes in allocator");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionFree(alloc) == 0, "has 0 free bytes in first region");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetRegionFree(alloc->next) == 0, "has 0 free bytes in second region");
  TEST_ASSERT_MESSAGE(allocation[size - 1] == 0, "grants access to full allocation");
  TEST_ASSERT_MESSAGE(second_allocation[size - 1] == 0, "grants access to full second allocation");

  ozoneAllocatorClear(alloc);
  TEST_ASSERT_MESSAGE(
      ozoneAllocatorGetTotalCapacity(alloc) == size * 2, "has total capacity equal to size after clear");
  TEST_ASSERT_MESSAGE(ozoneAllocatorGetTotalFree(alloc) == size * 2, "has total free space equal to size after clear");

  ozoneAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldReserveAndClearSingleRegion);
  RUN_TEST(shouldReserveAndClearMultipleRegions);
  return UNITY_END();
}
