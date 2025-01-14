#include "test.h"

#include "ozone_vector.h"

typedef int OzoneDummy;
OZONE_VECTOR_DECLARE_API(OzoneDummy)
OZONE_VECTOR_IMPLEMENT_API(OzoneDummy)

void shouldCreateAndPushToVector(void) {
  OzoneDummyVector vector = (OzoneDummyVector) { 0 };
  TEST_ASSERT_EQUAL_MESSAGE(0, ozoneVectorLength(&vector), "It sets the length to the correct value");

  int el_0 = 7;
  int el_1 = 14;
  int el_2 = 21;
  int el_3 = 28;
  int el_4 = 35;
  ozoneVectorPushOzoneDummy(test_alloc, &vector, &el_0);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, &el_1);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, &el_2);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, &el_3);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, &el_4);

  TEST_ASSERT_EQUAL_MESSAGE(8, vector.capacity, "It sets the capacity to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(5, ozoneVectorLength(&vector), "It sets the length to the correct value");

  TEST_ASSERT_EQUAL_MESSAGE(7, ozoneVectorAt(&vector, 0), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(14, ozoneVectorAt(&vector, 1), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(21, ozoneVectorAt(&vector, 2), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(28, ozoneVectorAt(&vector, 3), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(35, ozoneVectorAt(&vector, 4), "It sets the element to the correct value");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldCreateAndPushToVector);
  return UNITY_END();
}
