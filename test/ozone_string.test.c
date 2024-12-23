#include "test.h"

#include "ozone_string.h"

void shouldScanStringBuffer(void) {
  OzoneAllocatorT* alloc = ozoneAllocatorCreate(1024);

  OzoneStringT string = ozoneCharArray("hello, world!");
  OzoneStringT* scanned_stop_null = ozoneCharArrayScanBuffer(alloc, string.buffer, string.length, NULL);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(string, (*scanned_stop_null), "It scans the full buffer when stop is NULL.");

  OzoneStringT string_h = ozoneCharArray("h");
  OzoneStringT* scanned_stop_e = ozoneCharArrayScanBuffer(alloc, string.buffer, string.length, &ozoneCharArray("e"));
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(string_h, (*scanned_stop_e), "It scans only up to the stop character.");

  OzoneStringT string_hello = ozoneCharArray("hello");
  OzoneStringT* scanned_stop_space
      = ozoneCharArrayScanBuffer(alloc, string.buffer, string.length, &ozoneCharArray(", world"));
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(string_hello, (*scanned_stop_space), "It scans only up to the stop string.");

  ozoneAllocatorDelete(alloc);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldScanStringBuffer);
  return UNITY_END();
}
