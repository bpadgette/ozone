#ifndef OZONE_TEST_H
#define OZONE_TEST_H

#include "ozone_allocator.h"
#include "ozone_file.h"
#include "ozone_string.h"
#include "unity.h"

/** Context */
OzoneAllocator* test_alloc;
void setUp(void) { test_alloc = ozoneAllocatorCreate(1024); }
void tearDown(void) { ozoneAllocatorDelete(test_alloc); }

/** Resource handling */

OzoneString ozoneTestResourceLoadFromPath(const OzoneString* path) {
  OzoneStringVector chunks = ozoneFileLoadFromPath(test_alloc, path, OZONE_STRING_ENCODING_ISO_8859_1, 1024);
  return ozoneStringJoin(test_alloc, &chunks, OZONE_STRING_ENCODING_ISO_8859_1);
}

#define ozoneTestResource(_path_) ozoneTestResourceLoadFromPath(&ozoneString(_path_));

/** Test helpers */

#define TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(expected, actual, message)                                              \
  do {                                                                                                                 \
    TEST_ASSERT_EQUAL_MESSAGE(ozoneStringLength(expected), ozoneStringLength(actual), message " (length)");            \
    TEST_ASSERT_EQUAL_MESSAGE((expected)->encoding, (actual)->encoding, message "(encoding)");                         \
    TEST_ASSERT_EQUAL_STRING_MESSAGE(ozoneStringBuffer(expected), ozoneStringBuffer(actual), message " (buffer)");     \
  } while (0)

#endif
