#include "unity.h"

#define TEST_ASSERT_EQUAL_OZONE_STRING_MESSAGE(expected, actual, message)                                              \
  do {                                                                                                                 \
    TEST_ASSERT_EQUAL_MESSAGE(expected.length, actual.length, message " (length)");                                    \
    TEST_ASSERT_EQUAL_MESSAGE(expected.encoding, actual.encoding, message "(encoding)");                               \
    TEST_ASSERT_EQUAL_STRING_MESSAGE(expected.buffer, actual.buffer, message " (buffer)");                             \
  } while (0)

void setUp(void) { }
void tearDown(void) { }
