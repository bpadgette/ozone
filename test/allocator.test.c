#include "allocator.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void shouldReserveAndClearSingleRegion(void)
{
  int size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);
  char *allocation = ozAllocatorReserveMany(alloc, char, size);

  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalCapacity(alloc) == size, "allocates capacity equal to size");
  TEST_ASSERT_MESSAGE(ozAllocatorGetRegionCapacity(alloc) == size, "allocates capacity to single region");
  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalFree(alloc) == 0, "has 0 free bytes in allocator");
  TEST_ASSERT_MESSAGE(ozAllocatorGetRegionFree(alloc) == 0, "has 0 free bytes in allocator region");
  TEST_ASSERT_MESSAGE(allocation[size - 1] == 0, "grants access to full allocation");

  ozAllocatorClear(alloc);
  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalCapacity(alloc) == size, "has total capacity equal to size after clear");
  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalFree(alloc) == size, "has total free space equal to size after clear");

  ozAllocatorDelete(alloc);
}

void shouldReserveAndClearMultipleRegions(void)
{
  int size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  char *allocation = ozAllocatorReserveMany(alloc, char, size);
  char *second_allocation = ozAllocatorReserveMany(alloc, char, size);

  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalCapacity(alloc) == size * 2, "allocates capacity equal to size * 2");
  TEST_ASSERT_MESSAGE(ozAllocatorGetRegionCapacity(alloc) == size, "allocates capacity to first region");
  TEST_ASSERT_MESSAGE(ozAllocatorGetRegionCapacity(alloc->next) == size, "allocates capacity to second region");
  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalFree(alloc) == 0, "has 0 free bytes in allocator");
  TEST_ASSERT_MESSAGE(ozAllocatorGetRegionFree(alloc) == 0, "has 0 free bytes in first region");
  TEST_ASSERT_MESSAGE(ozAllocatorGetRegionFree(alloc->next) == 0, "has 0 free bytes in second region");
  TEST_ASSERT_MESSAGE(allocation[size - 1] == 0, "grants access to full allocation");
  TEST_ASSERT_MESSAGE(second_allocation[size - 1] == 0, "grants access to full second allocation");

  ozAllocatorClear(alloc);
  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalCapacity(alloc) == size * 2, "has total capacity equal to size after clear");
  TEST_ASSERT_MESSAGE(ozAllocatorGetTotalFree(alloc) == size * 2, "has total free space equal to size after clear");

  ozAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldReserveAndClearSingleRegion);
  RUN_TEST(shouldReserveAndClearMultipleRegions);
  return UNITY_END();
}
