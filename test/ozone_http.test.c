#include "test.h"

#include "ozone_http.h"
#include <string.h>

#define EXAMPLE_HTTP_REQUEST                                                                                           \
  "POST /users HTTP/1.1\r\n"                                                                                           \
  "Host: example.com\r\n"                                                                                              \
  "Content-Type: application/x-www-form-urlencoded\r\n"                                                                \
  "Content-Length: 27\r\n"                                                                                             \
  "\r\n"                                                                                                               \
  "field1=value1&field2=value2"

void shouldParseSocketChunksAsHTTPRequest(void) {
  OzoneString raw[] = { ozoneStringConstant(EXAMPLE_HTTP_REQUEST) };
  OzoneStringVector chunks = ozoneVectorFromArray(OzoneString, raw);

  OzoneHTTPRequest* request = ozoneHTTPParseSocketRequest(test_alloc, &chunks);
  TEST_ASSERT_NOT_NULL_MESSAGE(request, "It returns an HTTP request");

  TEST_ASSERT_EQUAL_MESSAGE(OZONE_HTTP_METHOD_POST, request->method, "It parses the correct HTTP method");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneStringConstant("/users"), request->target, "It parses the correct HTTP target");

  TEST_ASSERT_EQUAL_MESSAGE(OZONE_HTTP_VERSION_1_1, request->version, "It parses the correct HTTP version");

  const OzoneString* header = ozoneStringMapFindValue(request->headers, &ozoneStringConstant("Host"));
  TEST_ASSERT_NOT_NULL_MESSAGE(header, "It returns a Host header");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneStringConstant("example.com"), header, "It parses the correct Host value");

  header = ozoneStringMapFindValue(request->headers, &ozoneStringConstant("Content-Type"));
  TEST_ASSERT_NOT_NULL_MESSAGE(header, "It returns a Content-Type header");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneStringConstant("application/x-www-form-urlencoded"), header, "It parses the correct Content-Type value");

  header = ozoneStringMapFindValue(request->headers, &ozoneStringConstant("Content-Length"));
  TEST_ASSERT_NOT_NULL_MESSAGE(header, "It returns a Content-Length header");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(
      &ozoneStringConstant("27"), header, "It parses the correct Content-Length value");

  OzoneString body = ozoneStringConstant("field1=value1&field2=value2");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(&body, request->body, "It parses the correct HTTP body");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldParseSocketChunksAsHTTPRequest);
  return UNITY_END();
}
