#ifndef OZONE_LOG_H
#define OZONE_LOG_H
#include <libgen.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define ozLog(file, level, format, ...)                          \
  do                                                             \
  {                                                              \
    time_t seconds = time(NULL);                                 \
    struct tm *now = localtime(&seconds);                        \
    struct timeval now_ms;                                       \
    gettimeofday(&now_ms, NULL);                                 \
    fprintf(                                                     \
        file,                                                    \
        "{"                                                      \
        "\"level\":\"" level "\","                               \
        "\"timestamp\":\"%04d-%02d-%02d %02d:%02d:%02d.%03ld\"," \
        "\"message\":\"" format "\","                            \
        "\"location\":\"(%s) %s:%d\"}\r\n",                      \
        now->tm_year + 1900,                                     \
        now->tm_mon,                                             \
        now->tm_mday,                                            \
        now->tm_hour,                                            \
        now->tm_min,                                             \
        now->tm_sec,                                             \
        now_ms.tv_usec / 1000,                                   \
        ##__VA_ARGS__,                                           \
        __func__,                                                \
        basename(__FILE__),                                      \
        __LINE__);                                               \
    fflush(file);                                                \
  } while (0)

#define ozLogError(format, ...) ozLog(stderr, "error", format, ##__VA_ARGS__)
#define ozLogWarn(format, ...) ozLog(stderr, "warn", format, ##__VA_ARGS__)
#define ozLogInfo(format, ...) ozLog(stdout, "info", format, ##__VA_ARGS__)

#if defined(OZ_LOG_TRACE) || defined(OZ_LOG_DEBUG)
#define ozLogDebug(format, ...) ozLog(stdout, "debug", format, ##__VA_ARGS__)
#else
#define ozLogDebug(format, ...) \
  do                            \
  {                             \
  } while (0)
#endif

#ifdef OZ_LOG_TRACE
#define ozLogTrace(format, ...) ozLog(stdout, "trace", format, ##__VA_ARGS__)
#else
#define ozLogTrace(format, ...) \
  do                            \
  {                             \
  } while (0)
#endif

#endif
