#include "allocator.h"
#include "array.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void shouldCreateZeroedArray(void)
{
  int size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZArrayT *array = ozArrayCreate(alloc, 12);
  array->data = "hello";

  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "hello", "allocates a zero-allocated data section");

  ozAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldCreateZeroedArray);
  return UNITY_END();
}
