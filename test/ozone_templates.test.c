#include "test.h"

#include "ozone_file.h"
#include "ozone_templates.h"

void shouldParseTemplate(void) {
  OzoneStringVector source = ozoneFileLoadFromPath(test_alloc, &ozoneString("./test/resources/test.html"), 128);

  OzoneTemplatesComponent component = ozoneTemplatesComponentCreate(test_alloc, &ozoneString("test"), &source);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(&ozoneString("test"), &component.name, "It sets the component name");

  TEST_ASSERT_EQUAL_MESSAGE(5, ozoneVectorLength(&component.blocks), "It parses the template source into 5 blocks");
  TEST_ASSERT_EQUAL_MESSAGE(
      5, ozoneVectorLength(&component.block_classes), "It parses the template source into 5 blocks");

  TEST_ASSERT_EQUAL_MESSAGE(
      OZONE_TEMPLATES_BLOCK_CLASS_CONTENT,
      ozoneVectorAt(&component.block_classes, 0),
      "It parses the class of block 0");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("<!DOCTYPE html>\n"
                   "<html lang=\"en\">\n"
                   "<head>\n"
                   "    <meta charset=\"UTF-8\">\n"
                   "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                   "    <title>"),
      &ozoneVectorAt(&component.blocks, 0),
      "It parses the content of block 0");

  TEST_ASSERT_EQUAL_MESSAGE(
      OZONE_TEMPLATES_BLOCK_CLASS_NAMED, ozoneVectorAt(&component.block_classes, 1), "It parses the class of block 1");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("title"), &ozoneVectorAt(&component.blocks, 1), "It parses the content of block 1");

  TEST_ASSERT_EQUAL_MESSAGE(
      OZONE_TEMPLATES_BLOCK_CLASS_CONTENT,
      ozoneVectorAt(&component.block_classes, 2),
      "It parses the class of block 2");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("</title>\n"
                   "</head>\n"
                   "<body>\n"
                   "    "),
      &ozoneVectorAt(&component.blocks, 2),
      "It parses the content of block 2");

  TEST_ASSERT_EQUAL_MESSAGE(
      OZONE_TEMPLATES_BLOCK_CLASS_NAMED, ozoneVectorAt(&component.block_classes, 3), "It parses the class of block 3");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("body"), &ozoneVectorAt(&component.blocks, 3), "It parses the content of block 3");

  TEST_ASSERT_EQUAL_MESSAGE(
      OZONE_TEMPLATES_BLOCK_CLASS_CONTENT,
      ozoneVectorAt(&component.block_classes, 4),
      "It parses the class of block 4");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("\n"
                   "</body>\n"
                   "</html>\n"),
      &ozoneVectorAt(&component.blocks, 4),
      "It parses the content of block 4");
}

void shouldRenderTemplate(void) {
  OzoneTemplatesComponent component
      = ozoneTemplatesComponentFromFile(test_alloc, &ozoneString("./test/resources/test.html"));

  OzoneStringKeyValueVector template_arguments = (OzoneStringKeyValueVector) { 0 };
  ozoneStringPushKeyValue(test_alloc, &template_arguments, &ozoneString("title"), &ozoneString("Hello, World!"));
  ozoneStringPushKeyValue(
      test_alloc, &template_arguments, &ozoneString("body"), &ozoneString("Now available in Old High German."));

  OzoneString rendered = ozoneTemplatesComponentRender(test_alloc, &component, &template_arguments);
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneString("<!DOCTYPE html>\n"
                   "<html lang=\"en\">\n"
                   "<head>\n"
                   "    <meta charset=\"UTF-8\">\n"
                   "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                   "    <title>Hello, World!</title>\n"
                   "</head>\n"
                   "<body>\n"
                   "    Now available in Old High German.\n"
                   "</body>\n"
                   "</html>\n"),
      &rendered,
      "It should render a rendered template.");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldParseTemplate);
  RUN_TEST(shouldRenderTemplate);
  return UNITY_END();
}
