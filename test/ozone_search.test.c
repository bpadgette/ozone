#include "test.h"

#include "ozone_search.h"
#include "ozone_vector.h"

void shouldBinarySearch(void) {
  SizeTVector vector = ozoneVector(SizeT, 1, 4, 6, 8, 9, 13, 16, 35, 76, 80, 99, 100, 101, 200, 200, 200, 3424);

  TEST_ASSERT_EQUAL_MESSAGE(-1, ozoneVectorSearchBinary(SizeT, &vector, 0), "It does not find 0");
  TEST_ASSERT_EQUAL_MESSAGE(9, ozoneVectorSearchBinary(SizeT, &vector, 80), "It finds 80");
  TEST_ASSERT_EQUAL_MESSAGE(14, ozoneVectorSearchBinary(SizeT, &vector, 200), "It finds one of the 200s");
  TEST_ASSERT_EQUAL_MESSAGE(-1, ozoneVectorSearchBinary(SizeT, &vector, 5000), "It does not find 5000");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldBinarySearch);
  return UNITY_END();
}
