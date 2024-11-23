#ifndef OZONE_LOG_H
#define OZONE_LOG_H
#include <libgen.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define ozLog(file, level, format, format_args...)               \
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
        "\"function\":\"%s\","                                   \
        "\"location\":\"%s:%d\"}\r\n",                           \
        now->tm_year + 1900,                                     \
        now->tm_mon,                                             \
        now->tm_mday,                                            \
        now->tm_hour,                                            \
        now->tm_min,                                             \
        now->tm_sec,                                             \
        now_ms.tv_usec / 1000,                                   \
        ##format_args,                                           \
        __func__,                                                \
        basename(__FILE__),                                      \
        __LINE__);                                               \
    fflush(file);                                                \
  } while (0)

#define ozLogError(format, format_args...) ozLog(stderr, "error", format, ##format_args)
#define ozLogWarn(format, format_args...) ozLog(stderr, "warn", format, ##format_args)
#define ozLogInfo(format, format_args...) ozLog(stdout, "info", format, ##format_args)

#if defined(OZ_LOG_TRACE) || defined(OZ_LOG_DEBUG)
#define ozLogDebug(format, format_args...) ozLog(stdout, "debug", format, ##format_args)
#else
#define ozLogDebug(format, format_args...) \
  do                                       \
  {                                        \
  } while (0)
#endif

#ifdef OZ_LOG_TRACE
#define ozLogTrace(format, format_args...) ozLog(stdout, "trace", format, ##format_args)
#else
#define ozLogTrace(format, format_args...) \
  do                                       \
  {                                        \
  } while (0)
#endif

#endif
