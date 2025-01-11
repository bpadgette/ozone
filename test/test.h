#ifndef OZONE_TEST_H
#define OZONE_TEST_H

#include "ozone_allocator.h"
#include "ozone_file.h"
#include "ozone_string.h"
#include "unity.h"

/** Event */
OzoneAllocator* test_alloc;
void setUp(void) { test_alloc = ozoneAllocatorCreate(1024); }
void tearDown(void) { ozoneAllocatorDelete(test_alloc); }

/** Test helpers */

#define TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(expected, actual, message)                                              \
  do {                                                                                                                 \
    TEST_ASSERT_NOT_NULL_MESSAGE(actual, message " (was null)");                                                       \
    TEST_ASSERT_EQUAL_MESSAGE(ozoneStringLength(expected), ozoneStringLength(actual), message " (length)");            \
    TEST_ASSERT_EQUAL_STRING_MESSAGE(ozoneStringBuffer(expected), ozoneStringBuffer(actual), message " (buffer)");     \
  } while (0)

#endif
