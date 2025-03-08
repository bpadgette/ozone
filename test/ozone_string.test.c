#include "test.h"

#include "ozone_string.h"

void shouldWriteByteToString(void) {
  OzoneString string = ozoneString("hello, world!");
  ozoneStringWriteByte(test_alloc, &string, '!');
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("hello, world!!"), &string, "It appends a character to the string.");
}

void shouldFindInString(void) {
  OzoneString string = ozoneString("hello, world!");
  TEST_ASSERT_EQUAL_MESSAGE(
      1, ozoneStringFindFirst(&string, &ozoneString("el")), "It finds the first occurrence of the search string.");
  TEST_ASSERT_EQUAL_MESSAGE(
      -1,
      ozoneStringFindFirst(&string, &ozoneString("ele")),
      "It returns -1 when the first occurrence of a search string is not found.");

  TEST_ASSERT_EQUAL_MESSAGE(
      10, ozoneStringFindLast(&string, &ozoneString("l")), "It finds the last occurrence of the search string.");
  TEST_ASSERT_EQUAL_MESSAGE(
      -1,
      ozoneStringFindLast(&string, &ozoneString("ele")),
      "It returns -1 when the last occurrence of a search string is not found.");
}

void shouldScanStringBuffer(void) {
  char* string = "hello, world!";
  OzoneString* scanned = ozoneStringFromBuffer(test_alloc, string, 14);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(&ozoneString("hello, world!"), scanned, "It scans the full buffer.");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldWriteByteToString);
  RUN_TEST(shouldFindInString);
  RUN_TEST(shouldScanStringBuffer);
  return UNITY_END();
}
