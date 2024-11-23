#include "allocator.h"
#include "array.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void shouldCreateZeroedCharArray(void)
{
  int size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZCharArrayT *char_array = ozCharArrayCreate(alloc, 12);
  char_array->data = "hello";

  TEST_ASSERT_EQUAL_STRING_MESSAGE(char_array->data, "hello", "allocates a zero-allocated data section");

  ozAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldCreateZeroedCharArray);
  return UNITY_END();
}
