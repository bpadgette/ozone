#include "test.h"

#include "ozone_properties.h"

#define TEST_ASSERT_PROPERTY(_properties_, _key_, _value_)                                                             \
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(                                                                              \
      &ozoneString(_value_), OzoneStringMapFind(_properties_, &ozoneString(_key_)), "Testing key \"" _key_ "\"")

void shouldLoadPropertiesFromPath(void) {
  OzoneStringMap properties = (OzoneStringMap) { 0 };
  ozonePropertiesLoadFromPath(test_alloc, &properties, &ozoneString("./test/resources/wikipedia_example.properties"));

  TEST_ASSERT_PROPERTY(&properties, "website", "https://en.wikipedia.org/");
  TEST_ASSERT_PROPERTY(&properties, "language", "English");
  TEST_ASSERT_PROPERTY(&properties, "topic", ".properties files");
  TEST_ASSERT_PROPERTY(&properties, "empty", "");
  TEST_ASSERT_PROPERTY(&properties, "hello", "hello");
  TEST_ASSERT_PROPERTY(&properties, "whitespaceStart", " <-This space is not ignored.");
  TEST_ASSERT_PROPERTY(&properties, "duplicateKey", "second");
  TEST_ASSERT_PROPERTY(
      &properties, "delimiterCharacters:= ", "This is the value for the key \"delimiterCharacters:= \"");
  TEST_ASSERT_PROPERTY(&properties, "multiline", "This line continues");
  TEST_ASSERT_PROPERTY(&properties, "path", "c:\\wiki\\templates");
  TEST_ASSERT_PROPERTY(&properties, "evenKey", "This is on one line\\");
  TEST_ASSERT_PROPERTY(&properties, "oddKey", "This is line one and\\# This is line two");
  TEST_ASSERT_PROPERTY(&properties, "welcome", "Welcome to Wikipedia!");
  TEST_ASSERT_PROPERTY(&properties, "valueWithEscapes", "This is a newline\n and a carriage return\r and a tab\t.");

  TEST_ASSERT_EQUAL_MESSAGE(14, properties.keys.length, "the correct number of keys are inserted");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldLoadPropertiesFromPath);
  return UNITY_END();
}
