#include "allocator.h"
#include "array.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void shouldCreateZeroedArrayString(void)
{
  size_t size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZArrayStringT *array = ozArrayStringCreate(alloc, 12);
  array->data = "hello";

  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "hello", "allocates a zero-allocated data section");

  ozAllocatorDelete(alloc);
}

void shouldCopyArrayString(void)
{
  size_t size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZArrayStringT *array = ozArrayStringCreate(alloc, 6);
  array->data = "hello";
  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "hello", "creates original array");

  OZArrayStringT *copy = ozArrayStringCopy(alloc, array);
  array->data = "Hello";
  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "Hello", "changes affect original array");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(copy->data, "hello", "deep copies original array");

  ozAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldCreateZeroedArrayString);
  RUN_TEST(shouldCopyArrayString);
  return UNITY_END();
}
