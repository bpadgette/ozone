#include "ozone_allocator.h"
#include "ozone_http.h"
#include "unity.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void shouldCreateHTTPHeaders(void)
{
  size_t size = 1024;
  OzoneAllocatorT *alloc = ozoneAllocatorCreate(size);

  OzoneArrayStringT new_keys[1] = {(OzoneArrayStringT){.data = "content-type", .length = 13}};
  OzoneArrayStringT new_values[1] = {(OzoneArrayStringT){.data = "text/plain", .length = 11}};
  OzoneHTTPHeadersT *headers = ozoneHTTPHeadersSetHeaders(alloc, NULL, new_keys, new_values, 1);

  TEST_ASSERT_MESSAGE(headers->count == 1, "set new header count");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[0].data, headers->keys[0].data, "set new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[0].data, headers->values[0].data, "set new header value");

  ozoneAllocatorDelete(alloc);
}

void shouldOverwriteHTTPHeaders(void)
{
  size_t size = 1024;
  OzoneAllocatorT *alloc = ozoneAllocatorCreate(size);

  OzoneArrayStringT new_keys[2] = {(OzoneArrayStringT){.data = "content-type", .length = 13}, (OzoneArrayStringT){.data = "authorization", .length = 14}};
  OzoneArrayStringT new_values[2] = {(OzoneArrayStringT){.data = "text/plain", .length = 11}, (OzoneArrayStringT){.data = "Bearer token", .length = 13}};
  OzoneHTTPHeadersT *headers = ozoneHTTPHeadersSetHeaders(alloc, NULL, new_keys, new_values, 2);

  TEST_ASSERT_MESSAGE(headers->count == 2, "set new header count");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[0].data, headers->keys[0].data, "set new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[0].data, headers->values[0].data, "set new header value");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[1].data, headers->keys[1].data, "set second new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[1].data, headers->values[1].data, "set second new header value");

  OzoneArrayStringT overwrite_key[1] = {(OzoneArrayStringT){.data = "content-type", .length = 13}};
  OzoneArrayStringT overwrite_value[1] = {(OzoneArrayStringT){.data = "text/html", .length = 10}};
  ozoneHTTPHeadersSetHeaders(alloc, headers, overwrite_key, overwrite_value, 1);

  TEST_ASSERT_MESSAGE(headers->count == 2, "not set new header count");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(overwrite_key[0].data, headers->keys[0].data, "not set new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(overwrite_value[0].data, headers->values[0].data, "set new header value");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[1].data, headers->keys[1].data, "not set second new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[1].data, headers->values[1].data, "not set second new header value");

  ozoneAllocatorDelete(alloc);
}

void shouldCreateHTTPRequestFromString(void)
{
  size_t size = 1024;
  OzoneAllocatorT *alloc = ozoneAllocatorCreate(size);

  OzoneArrayStringT string = (OzoneArrayStringT){
      .data = "POST /users HTTP/1.1\r\n"
              "Host: example.com\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
              "Content-Length: 11\r\n"
              "\r\n"
              "hello=world"
              "\r\n"
              "\r\n",
      .length = 124};

  OzoneHTTPRequestT *request = ozoneHTTPRequestCreateFromString(alloc, &string);
  TEST_ASSERT_EQUAL_STRING_MESSAGE("POST", request->method.data, "set method");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("/users", request->target.data, "set target");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("HTTP/1.1", request->version.data, "set version");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("Host", request->headers.keys[0].data, "set header 1");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("example.com", request->headers.values[0].data, "set header 1");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("Content-Type", request->headers.keys[1].data, "set header 2");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("application/x-www-form-urlencoded", request->headers.values[1].data, "set header 2");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("Content-Length", request->headers.keys[2].data, "set header 3");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("11", request->headers.values[2].data, "set header 3");
  TEST_ASSERT_EQUAL_STRING_MESSAGE("hello=world", request->body.data, "set body");

  ozoneAllocatorDelete(alloc);
}

void shouldCreateStringFromHTTPResponse(void)
{
  size_t size = 1024;
  OzoneAllocatorT *alloc = ozoneAllocatorCreate(size);

  OzoneHTTPResponseT *response = ozoneHTTPResponseCreate(alloc);
  response->code = 200;
  response->body = ozoneArrayStringFromChars("ooo");
  response->headers = (OzoneHTTPHeadersT){
      .count = 1,
      .keys = &ozoneArrayStringFromChars("Content-Type"),
      .values = &ozoneArrayStringFromChars("text/plain")};

  OzoneArrayStringT *string = ozoneHTTPResponseGetString(alloc, response);
  TEST_ASSERT_EQUAL_STRING_MESSAGE(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 3\r\n\r\n"
      "ooo\r\n",
      string->data,
      "create HTTP string from response");

  ozoneAllocatorDelete(alloc);
}

int main(void)
{
  UNITY_BEGIN();
  RUN_TEST(shouldCreateHTTPHeaders);
  RUN_TEST(shouldOverwriteHTTPHeaders);
  RUN_TEST(shouldCreateHTTPRequestFromString);
  RUN_TEST(shouldCreateStringFromHTTPResponse);
  return UNITY_END();
}
