#include "test.h"

#include "ozone_string.h"

void shouldAppendToString(void) {
  OzoneString string = ozoneStringConstant("hello, world!");
  ozoneStringAppend(test_alloc, &string, '!');
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneStringConstant("hello, world!!"), &string, "It appends a character to the string.");
}

void shouldFindInString(void) {
  OzoneString string = ozoneStringConstant("hello, world!");
  TEST_ASSERT_EQUAL_MESSAGE(
      1,
      ozoneStringFindFirst(&string, &ozoneStringConstant("el")),
      "It finds the first occurrence of the search string.");
  TEST_ASSERT_EQUAL_MESSAGE(
      -1,
      ozoneStringFindFirst(&string, &ozoneStringConstant("ele")),
      "It returns -1 when the search string is not found.");
}

void shouldScanStringBuffer(void) {
  char* string = "hello, world!";
  OzoneString* scanned = ozoneStringFromBuffer(test_alloc, string, 14);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(&ozoneStringConstant("hello, world!"), scanned, "It scans the full buffer.");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldAppendToString);
  RUN_TEST(shouldFindInString);
  RUN_TEST(shouldScanStringBuffer);
  return UNITY_END();
}
