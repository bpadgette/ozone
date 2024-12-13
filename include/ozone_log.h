#ifndef OZONE_LOG_H
#define OZONE_LOG_H
#include <libgen.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define ozoneLog(file, level, format, ...)                                                                             \
  do {                                                                                                                 \
    time_t seconds = time(NULL);                                                                                       \
    struct tm* now = localtime(&seconds);                                                                              \
    struct timeval now_ms;                                                                                             \
    gettimeofday(&now_ms, NULL);                                                                                       \
    fprintf(file,                                                                                                      \
        "{"                                                                                                            \
        "\"level\":\"" level "\","                                                                                     \
        "\"timestamp\":\"%04d-%02d-%02d %02d:%02d:%02d.%03ld\","                                                       \
        "\"message\":\"" format "\","                                                                                  \
        "\"location\":\"(%s) %s:%d\"}\r\n",                                                                            \
        now->tm_year + 1900, now->tm_mon, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, now_ms.tv_usec / 1000, \
        ##__VA_ARGS__, __func__, basename(__FILE__), __LINE__);                                                        \
    fflush(file);                                                                                                      \
  } while (0)

#define ozoneLogError(format, ...) ozoneLog(stderr, "error", format, ##__VA_ARGS__)
#define ozoneLogWarn(format, ...) ozoneLog(stderr, "warn", format, ##__VA_ARGS__)
#define ozoneLogInfo(format, ...) ozoneLog(stdout, "info", format, ##__VA_ARGS__)

#if defined(OZONE_LOG_TRACE) || defined(OZONE_LOG_DEBUG)
#define ozoneLogDebug(format, ...) ozoneLog(stdout, "debug", format, ##__VA_ARGS__)
#else
#define ozoneLogDebug(format, ...)                                                                                     \
  do {                                                                                                                 \
  } while (0)
#endif

#ifdef OZONE_LOG_TRACE
#define ozoneLogTrace(format, ...) ozoneLog(stdout, "trace", format, ##__VA_ARGS__)
#else
#define ozoneLogTrace(format, ...)                                                                                     \
  do {                                                                                                                 \
  } while (0)
#endif

#endif
