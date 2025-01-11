#include "test.h"

#include "ozone_vector.h"

typedef int OzoneDummy;
OZONE_VECTOR_DECLARE_API(OzoneDummy)
OZONE_VECTOR_IMPLEMENT_API(OzoneDummy)

void shouldCreateAndPushToVector(void) {
  OzoneDummyVector vector = (OzoneDummyVector) { 0 };
  TEST_ASSERT_EQUAL_MESSAGE(0, ozoneVectorLength(&vector), "It sets the length to the correct value");

  pushOzoneDummy(test_alloc, &vector, 7);
  pushOzoneDummy(test_alloc, &vector, 14);
  pushOzoneDummy(test_alloc, &vector, 21);
  pushOzoneDummy(test_alloc, &vector, 28);
  pushOzoneDummy(test_alloc, &vector, 35);

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
