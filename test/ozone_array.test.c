#include "ozone_array.h"
#include "ozone_allocator.h"
#include "unity.h"

void setUp(void) { }

void tearDown(void) { }

void shouldCreateZeroedCharArray(void)
{
  size_t size = 1024;
  OzoneAllocatorT* alloc = ozoneAllocatorCreate(size);

  OzoneCharArrayT* array = ozoneCharArrayCreate(alloc, 12);
  array->data = "hello";

  TEST_ASSERT_EQUAL_STRING_MESSAGE(array->data, "hello", "allocates a zero-allocated data section");

  ozoneAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldCreateZeroedCharArray);
  return UNITY_END();
}
