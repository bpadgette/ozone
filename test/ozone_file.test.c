#include "test.h"

#include "ozone_file.h"

void shouldLoadFileFromPath(void) {
  OzoneStringTVectorT chunks = ozoneFileLoadFromPath(
      test_alloc, &ozoneString("./test/resources/content.txt"), OZONE_STRING_ENCODING_ISO_8859_1, 100);

  TEST_ASSERT_EQUAL_MESSAGE(1, chunks.length, "It should load the whole file into a single chunk");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(&ozoneString("Hello,\n\n\tWorld!\n\nfin\n"), &chunks.elements[0],
      "It should load all of the file's content into the first chunk");
}

void shouldLoadFileChunksFromPath(void) {
  OzoneStringTVectorT chunks = ozoneFileLoadFromPath(
      test_alloc, &ozoneString("./test/resources/content.txt"), OZONE_STRING_ENCODING_ISO_8859_1, 10);

  TEST_ASSERT_EQUAL_MESSAGE(3, chunks.length, "It should load the whole file into 3 chunks");

  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("Hello,\n\n\t"), &chunks.elements[0], "It should load some of the file's content into chunk 0");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("World!\n\nf"), &chunks.elements[1], "It should load some of the file's content into chunk 1");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("in\n"), &chunks.elements[2], "It should load some of the file's content into chunk 2");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldLoadFileFromPath);
  RUN_TEST(shouldLoadFileChunksFromPath);
  return UNITY_END();
}
