#ifndef OZONE_TIME_H
#define OZONE_TIME_H

#include <sys/time.h>

long int ozoneTimeDifferenceMilliseconds(struct timeval* begin, struct timeval* end);

#endif
