#include "test.h"

#include "ozone_http.h"
#include <string.h>

const char* EXAMPLE_HTTP_REQUEST = {
  "POST /users HTTP/1.1\r\n"
  "Host: example.com\r\n"
  "Content-Type: application/x-www-form-urlencoded\r\n"
  "Content-Length: 27\r\n"
  "\r\n"
  "field1=value1&field2=value2",
};

void shouldParseSocketChunksAsHTTPRequest(void) {
  OzoneAllocatorT* alloc = ozoneAllocatorCreate(1024);

  OzoneSocketChunkT chunk = (OzoneSocketChunkT) {
    .buffer = ozoneAllocatorReserveMany(alloc, char, 512),
    .length = 512,
  };
  memcpy(chunk.buffer, EXAMPLE_HTTP_REQUEST, chunk.length);

  OzoneHTTPRequestT* request = ozoneHTTPParseSocketChunks(alloc, &chunk);
  TEST_ASSERT_NOT_NULL_MESSAGE(request, "It returns an HTTP request");

  TEST_ASSERT_EQUAL_MESSAGE(OZONE_HTTP_METHOD_POST, request->method, "It parses the correct HTTP method");
  OzoneStringT target = ozoneCharArray("/users");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(target, request->target, "It parses the correct HTTP target");

  TEST_ASSERT_EQUAL_MESSAGE(OZONE_HTTP_VERSION_1_1, request->version, "It parses the correct HTTP version");

  OzoneStringT value = ozoneCharArray("example.com");
  OzoneStringT* header = ozoneHTTPGetHeaderValue(request->headers, ozoneCharArray("Host"));
  TEST_ASSERT_NOT_NULL_MESSAGE(header, "It returns a Host header");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(value, (*header), "It parses the correct Host value");

  value = ozoneCharArray("application/x-www-form-urlencoded");
  header = ozoneHTTPGetHeaderValue(request->headers, ozoneCharArray("Content-Type"));
  TEST_ASSERT_NOT_NULL_MESSAGE(header, "It returns a Content-Type header");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(value, (*header), "It parses the correct Content-Type value");

  value = ozoneCharArray("27");
  header = ozoneHTTPGetHeaderValue(request->headers, ozoneCharArray("Content-Length"));
  TEST_ASSERT_NOT_NULL_MESSAGE(header, "It returns a Content-Length header");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(value, (*header), "It parses the correct Content-Length value");

  OzoneStringT body = ozoneCharArray("field1=value1&field2=value2");
  TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(body, request->body, "It parses the correct HTTP body");

  ozoneAllocatorDelete(alloc);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldParseSocketChunksAsHTTPRequest);
  return UNITY_END();
}
