#include "test.h"

#include "ozone_file.h"

void shouldLoadFileFromPath(void) {
  OzoneStringVector chunks = (OzoneStringVector) { 0 };
  ozoneFileLoadFromPath(test_alloc, &chunks, &ozoneString("./test/resources/content.txt"), 100);

  TEST_ASSERT_EQUAL_MESSAGE(1, ozoneVectorLength(&chunks), "It should load the whole file into a single chunk");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("Hello,\n\n\tWorld!\n\nfin\n"),
      &ozoneVectorAt(&chunks, 0),
      "It should load all of the file's content into the first chunk");
}

void shouldLoadFileChunksFromPath(void) {
  OzoneStringVector chunks = (OzoneStringVector) { 0 };
  ozoneFileLoadFromPath(test_alloc, &chunks, &ozoneString("./test/resources/content.txt"), 10);

  TEST_ASSERT_EQUAL_MESSAGE(3, ozoneVectorLength(&chunks), "It should load the whole file into 3 chunks");

  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("Hello,\n\n\t"),
      &ozoneVectorAt(&chunks, 0),
      "It should load some of the file's content into chunk 0");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("World!\n\nf"),
      &ozoneVectorAt(&chunks, 1),
      "It should load some of the file's content into chunk 1");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("in\n"), &ozoneVectorAt(&chunks, 2), "It should load some of the file's content into chunk 2");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldLoadFileFromPath);
  RUN_TEST(shouldLoadFileChunksFromPath);
  return UNITY_END();
}
