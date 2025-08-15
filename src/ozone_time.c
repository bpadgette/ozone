#include "ozone_time.h"

long int ozoneTimeDifferenceMilliseconds(struct timeval* begin, struct timeval* end) {
  long int milliseconds = 0;

  if (end) {
    milliseconds += end->tv_sec * 1000;
    milliseconds += end->tv_usec ? end->tv_usec / 1000 : 0;
  }

  if (begin) {
    milliseconds -= begin->tv_sec * 1000;
    milliseconds -= begin->tv_usec ? begin->tv_usec / 1000 : 0;
  }

  return milliseconds;
}
