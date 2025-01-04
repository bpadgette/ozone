#include "test.h"

#include "ozone_vector.h"

typedef int OzoneDummy;
OZONE_VECTOR_DECLARE_API(OzoneDummy)
OZONE_VECTOR_IMPLEMENT_API(OzoneDummy)

void shouldCreateAndPushToVector(void) {
  OzoneDummyVector vector = ozoneVector(test_alloc, OzoneDummy, 3);
  TEST_ASSERT_EQUAL_MESSAGE(3, vector.capacity, "It sets the capacity to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(3, vector.capacity_increment, "It sets the capacity increment to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(0, vector.length, "It sets the length to the correct value");
  TEST_ASSERT_NOT_NULL_MESSAGE(vector.elements, "It reserves the vector's elements");

  ozoneVectorPushOzoneDummy(test_alloc, &vector, 7);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, 14);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, 21);
  ozoneVectorPushOzoneDummy(test_alloc, &vector, 28);

  TEST_ASSERT_EQUAL_MESSAGE(6, vector.capacity, "It sets the capacity to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(3, vector.capacity_increment, "It sets the capacity increment to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(4, vector.length, "It sets the length to the correct value");

  TEST_ASSERT_EQUAL_MESSAGE(7, vector.elements[0], "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(14, vector.elements[1], "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(21, vector.elements[2], "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(28, vector.elements[3], "It sets the element to the correct value");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldCreateAndPushToVector);
  return UNITY_END();
}
