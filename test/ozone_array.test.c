#include "ozone_allocator.h"
#include "ozone_array.h"
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
  OzoneAllocatorT *alloc = ozoneAllocatorCreate(size);

  OzoneArrayStringT *array = ozoneArrayStringCreate(alloc, 12);
  array->data = "hello";

  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "hello", "allocates a zero-allocated data section");

  ozoneAllocatorDelete(alloc);
}

void shouldCopyArrayString(void)
{
  size_t size = 1024;
  OzoneAllocatorT *alloc = ozoneAllocatorCreate(size);

  OzoneArrayStringT *array = ozoneArrayStringCreate(alloc, 6);
  array->data = "hello";
  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "hello", "creates original array");

  OzoneArrayStringT *copy = ozoneArrayStringCopy(alloc, array);
  array->data = "Hello";
  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "Hello", "changes affect original array");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(copy->data, "hello", "deep copies original array");

  ozoneAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldCreateZeroedArrayString);
  RUN_TEST(shouldCopyArrayString);
  return UNITY_END();
}
