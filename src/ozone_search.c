#include "ozone_search.h"

long ozoneSearchBinaryInteger(void* array, long array_length, unsigned short integer_size, long search) {
  long left = 0;
  long right = array_length - 1;
  while (left <= right) {
    long middle = left + ((right - left) / 2);
    long* middle_value = (long*)((char*)array + middle * integer_size);

    if (*middle_value < search) {
      left = middle + 1;
    } else if (*middle_value > search) {
      right = middle - 1;
    } else {
      return middle;
    }
  }

  return -1;
}
