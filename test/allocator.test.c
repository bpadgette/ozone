#include "allocator.h"
#include "unity.h"

Allocator *strict_alloc;

void setUp(void)
{
  strict_alloc = createAllocator(512);
}

void tearDown(void)
{
  deleteAllocator(strict_alloc);
}

void test_pushAllocator_ShouldAllocateZeroes(void)
{
  int length = 16 * 1024;

  char *allocation = pushAllocator(strict_alloc, char, length);
  TEST_ASSERT_NOT_NULL(allocation);

  for (int i = 0; i < length; i++)
  {
    TEST_ASSERT_EQUAL(0, allocation[i]);
  }
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(test_pushAllocator_ShouldAllocateZeroes);
  return UNITY_END();
}
