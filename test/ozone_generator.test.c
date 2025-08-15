#include "test.h"

#include "ozone_generator.h"

typedef struct OzoneTestIteratorStruct {
  size_t at_index;
  size_t stop_index;
  size_t value;
} OzoneTestIterator;

typedef struct OzoneTestRejectStruct {
  int should_reject;
} OzoneTestReject;

OZONE_GENERATOR_DECLARE_API(OzoneTestIterator)
OZONE_GENERATOR_DECLARE_API(OzoneTestReject)

void testNextIterator(OzoneTestIteratorGenerator* generator) {
  ozoneGeneratorBegin(generator);

  for (generator->context.at_index = 0; generator->context.at_index < generator->context.stop_index;
       generator->context.at_index++) {

    generator->context.value++;
    ozoneGeneratorYield(generator);

    generator->context.value++;
    ozoneGeneratorYield(generator);
  }

  ozoneGeneratorResolve(generator);
}

void testNextRejection(OzoneTestRejectGenerator* generator) {
  ozoneGeneratorBegin(generator);

  ozoneGeneratorYield(generator);

  if (generator->context.should_reject)
    ozoneGeneratorReject(generator);

  ozoneGeneratorResolve(generator);
}

void shouldYield(void) {
  OzoneTestIteratorGenerator generator = ozoneGenerator(OzoneTestIterator, (OzoneTestIterator) { .stop_index = 3 });

  for (int expected = 0; ozoneGeneratorPending(&generator); expected++) {
    TEST_ASSERT_EQUAL_MESSAGE(expected, generator.context.value, "It generates and yields");
    testNextIterator(&generator);
  }

  TEST_ASSERT_EQUAL_MESSAGE(generator.context.at_index, generator.context.stop_index, "It exits the pending state");
  int end = generator.context.value;

  testNextIterator(&generator);
  testNextIterator(&generator);
  testNextIterator(&generator);
  TEST_ASSERT_EQUAL_MESSAGE(end, generator.context.value, "It stops generating");
}

void shouldResolveOrReject(void) {
  OzoneTestRejectGenerator generator = ozoneGenerator(OzoneTestReject, (OzoneTestReject) { .should_reject = 0 });
  TEST_ASSERT_EQUAL_MESSAGE(OZONE_GENERATOR_PENDING, generator.state, "It is pending before the first step");

  testNextRejection(&generator);
  TEST_ASSERT_EQUAL_MESSAGE(OZONE_GENERATOR_PENDING, generator.state, "It is pending after the first step");

  testNextRejection(&generator);
  TEST_ASSERT_EQUAL_MESSAGE(OZONE_GENERATOR_RESOLVED, generator.state, "It is resolved after the second step");

  generator = ozoneGenerator(OzoneTestReject, (OzoneTestReject) { .should_reject = 1 });
  TEST_ASSERT_EQUAL_MESSAGE(OZONE_GENERATOR_PENDING, generator.state, "It is pending before the first step");

  testNextRejection(&generator);
  TEST_ASSERT_EQUAL_MESSAGE(OZONE_GENERATOR_PENDING, generator.state, "It is pending after the first step");

  testNextRejection(&generator);
  TEST_ASSERT_EQUAL_MESSAGE(OZONE_GENERATOR_REJECTED, generator.state, "It is rejected after the second step");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(shouldYield);
  RUN_TEST(shouldResolveOrReject);
  return UNITY_END();
}
