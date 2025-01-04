#include "test.h"

#include "ozone_string.h"

void shouldAppendToString(void) {
  OzoneString string = ozoneString("hello, world!");
  ozoneStringAppendChar(test_alloc, &string, '!');
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("hello, world!!"), &string, "It appends a character to the string.");
}

void shouldScanStringBuffer(void) {
  char* string = "hello, world!";
  OzoneString scanned_stop_null = ozoneStringScanBuffer(test_alloc, string, 14, NULL, OZONE_STRING_ENCODING_ISO_8859_1);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("hello, world!"), &scanned_stop_null, "It scans the full buffer when stop is NULL.");

  OzoneString scanned_stop_e
      = ozoneStringScanBuffer(test_alloc, string, 14, &ozoneString("e"), OZONE_STRING_ENCODING_ISO_8859_1);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(&ozoneString("h"), &scanned_stop_e, "It scans only up to the stop character.");

  OzoneString scanned_stop_space
      = ozoneStringScanBuffer(test_alloc, string, 14, &ozoneString(", world"), OZONE_STRING_ENCODING_ISO_8859_1);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("hello"), &scanned_stop_space, "It scans only up to the stop string.");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldAppendToString);
  RUN_TEST(shouldScanStringBuffer);
  return UNITY_END();
}
