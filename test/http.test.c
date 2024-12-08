#include "allocator.h"
#include "http.h"
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
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZArrayStringT new_keys[1] = {(OZArrayStringT){.data = "content-type", .length = 13}};
  OZArrayStringT new_values[1] = {(OZArrayStringT){.data = "text/plain", .length = 11}};
  OZHTTPHeadersT *headers = ozHTTPHeadersSetHeaders(alloc, NULL, new_keys, new_values, 1);

  TEST_ASSERT_MESSAGE(headers->count == 1, "set new header count");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[0].data, headers->keys[0].data, "set new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[0].data, headers->values[0].data, "set new header value");

  ozAllocatorDelete(alloc);
}

void shouldOverwriteHTTPHeaders(void)
{
  size_t size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZArrayStringT new_keys[2] = {(OZArrayStringT){.data = "content-type", .length = 13}, (OZArrayStringT){.data = "authorization", .length = 14}};
  OZArrayStringT new_values[2] = {(OZArrayStringT){.data = "text/plain", .length = 11}, (OZArrayStringT){.data = "Bearer token", .length = 13}};
  OZHTTPHeadersT *headers = ozHTTPHeadersSetHeaders(alloc, NULL, new_keys, new_values, 2);

  TEST_ASSERT_MESSAGE(headers->count == 2, "set new header count");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[0].data, headers->keys[0].data, "set new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[0].data, headers->values[0].data, "set new header value");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[1].data, headers->keys[1].data, "set second new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[1].data, headers->values[1].data, "set second new header value");

  OZArrayStringT overwrite_key[1] = {(OZArrayStringT){.data = "content-type", .length = 13}};
  OZArrayStringT overwrite_value[1] = {(OZArrayStringT){.data = "text/html", .length = 10}};
  ozHTTPHeadersSetHeaders(alloc, headers, overwrite_key, overwrite_value, 1);

  TEST_ASSERT_MESSAGE(headers->count == 2, "not set new header count");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(overwrite_key[0].data, headers->keys[0].data, "not set new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(overwrite_value[0].data, headers->values[0].data, "set new header value");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_keys[1].data, headers->keys[1].data, "not set second new header key");
  TEST_ASSERT_EQUAL_STRING_MESSAGE(new_values[1].data, headers->values[1].data, "not set second new header value");

  ozAllocatorDelete(alloc);
}

void shouldCreateHTTPRequestFromString(void)
{
  size_t size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZArrayStringT string = (OZArrayStringT){
      .data = "POST /users HTTP/1.1\r\n"
              "Host: example.com\r\n"
              "Content-Type: application/x-www-form-urlencoded\r\n"
              "Content-Length: 11\r\n"
              "\r\n"
              "hello=world"
              "\r\n"
              "\r\n",
      .length = 124};

  OZHTTPRequestT *request = ozHTTPRequestCreateFromString(alloc, &string);
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

  ozAllocatorDelete(alloc);
}

void shouldCreateStringFromHTTPResponse(void)
{
  size_t size = 1024;
  OZAllocatorT *alloc = ozAllocatorCreate(size);

  OZHTTPResponseT *response = ozHTTPResponseCreate(alloc);
  response->code = 200;
  response->body = ozArrayStringFromChars("ooo");
  response->headers = (OZHTTPHeadersT){
      .count = 1,
      .keys = &ozArrayStringFromChars("Content-Type"),
      .values = &ozArrayStringFromChars("text/plain")};

  OZArrayStringT *string = ozHTTPResponseGetString(alloc, response);
  TEST_ASSERT_EQUAL_STRING_MESSAGE(
      "HTTP/1.1 200 OK\r\n"
      "Content-Type: text/plain\r\n"
      "Content-Length: 3\r\n\r\n"
      "ooo\r\n",
      string->data,
      "create HTTP string from response");

  ozAllocatorDelete(alloc);
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
