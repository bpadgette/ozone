#include "test.h"

#include "ozone_vector.h"

OZONE_VECTOR_DECLARE_API(int)
OZONE_VECTOR_IMPLEMENT_API(int)

void shouldFulfillVectorOperations(void) {
  intVector vector = (intVector) { 0 };
  TEST_ASSERT_EQUAL_MESSAGE(0, vector.length, "It sets the length to the correct value");

  int el_0 = 7;
  int el_1 = 14;
  int el_2 = 21;
  int el_3 = 28;
  int el_4 = 35;
  intVectorPush(test_alloc, &vector, &el_0);
  intVectorPush(test_alloc, &vector, &el_1);
  intVectorPush(test_alloc, &vector, &el_2);
  intVectorPush(test_alloc, &vector, &el_3);
  intVectorPush(test_alloc, &vector, &el_4);

  TEST_ASSERT_EQUAL_MESSAGE(8, vector.capacity, "It sets the capacity to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(5, vector.length, "It sets the length to the correct value");

  TEST_ASSERT_EQUAL_MESSAGE(7, ozoneVectorAt(&vector, 0), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(14, ozoneVectorAt(&vector, 1), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(21, ozoneVectorAt(&vector, 2), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(28, ozoneVectorAt(&vector, 3), "It sets the element to the correct value");
  TEST_ASSERT_EQUAL_MESSAGE(35, ozoneVectorAt(&vector, 4), "It sets the element to the correct value");

  intVectorPop(&vector, NULL);
  TEST_ASSERT_EQUAL_MESSAGE(8, vector.capacity, "It does not alter capacity");
  TEST_ASSERT_EQUAL_MESSAGE(4, vector.length, "It reduces length");

  intVectorPush(test_alloc, &vector, &el_4);
  TEST_ASSERT_EQUAL_MESSAGE(35, ozoneVectorAt(&vector, 4), "It sets the element to the correct value");

  int popped_0 = 0;
  intVectorPop(&vector, &popped_0);
  TEST_ASSERT_EQUAL_MESSAGE(8, vector.capacity, "It does not alter capacity");
  TEST_ASSERT_EQUAL_MESSAGE(4, vector.length, "It reduces length");
  TEST_ASSERT_EQUAL_MESSAGE(35, popped_0, "It sets the element to the correct value");

  intVectorPush(test_alloc, &vector, &el_4);
  TEST_ASSERT_EQUAL_MESSAGE(35, ozoneVectorAt(&vector, 4), "It sets the element to the correct value");

  int shifted_0 = 0;
  intVectorShift(&vector, &shifted_0);
  TEST_ASSERT_EQUAL_MESSAGE(7, vector.capacity, "It reduces capacity");
  TEST_ASSERT_EQUAL_MESSAGE(4, vector.length, "It reduces length");
  TEST_ASSERT_EQUAL_MESSAGE(7, shifted_0, "It sets the element to the correct value");

  intVectorPush(test_alloc, &vector, &el_4);
  TEST_ASSERT_EQUAL_MESSAGE(35, ozoneVectorAt(&vector, 4), "It sets the element to the correct value");

  int* block_start = vector.elements_block_begin;
  TEST_ASSERT_EQUAL_MESSAGE(1, vector.elements - block_start, "It creeps elements forward after shifting");

  intVectorPush(test_alloc, &vector, &el_4);
  TEST_ASSERT_EQUAL_MESSAGE(1, vector.elements - block_start, "It does not move elements after pushing once");

  intVectorPush(test_alloc, &vector, &el_4);
  intVectorPush(test_alloc, &vector, &el_4);
  intVectorPush(test_alloc, &vector, &el_4);
  intVectorPush(test_alloc, &vector, &el_4);
  intVectorPush(test_alloc, &vector, &el_4);
  intVectorPush(test_alloc, &vector, &el_4);
  TEST_ASSERT_EQUAL_MESSAGE(0, vector.elements - block_start, "It does reclaim space after the capacity grows");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldFulfillVectorOperations);
  return UNITY_END();
}
