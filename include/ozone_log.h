#ifndef OZONE_LOG_H
#define OZONE_LOG_H
#include <libgen.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#define ozoneLog(file, level, ...)                                                                                     \
  do {                                                                                                                 \
    time_t seconds = time(NULL);                                                                                       \
    struct tm* now = localtime(&seconds);                                                                              \
    struct timeval now_ms;                                                                                             \
    gettimeofday(&now_ms, NULL);                                                                                       \
    fprintf(                                                                                                           \
        file,                                                                                                          \
        "{"                                                                                                            \
        "\"level\":\"" level "\","                                                                                     \
        "\"timestamp\":\"%04d-%02d-%02d %02d:%02d:%02d.%03d\","                                                        \
        "\"message\":\"",                                                                                              \
        now->tm_year + 1900,                                                                                           \
        now->tm_mon + 1,                                                                                               \
        now->tm_mday,                                                                                                  \
        now->tm_hour,                                                                                                  \
        now->tm_min,                                                                                                   \
        now->tm_sec,                                                                                                   \
        (int)now_ms.tv_usec / 1000);                                                                                   \
    fprintf(file, __VA_ARGS__);                                                                                        \
    fprintf(                                                                                                           \
        file,                                                                                                          \
        "\","                                                                                                          \
        "\"location\":\"(%s) %s:%d\"}\r\n",                                                                            \
        __func__,                                                                                                      \
        basename(__FILE__),                                                                                            \
        __LINE__);                                                                                                     \
    fflush(file);                                                                                                      \
  } while (0)

#define ozoneLogError(...) ozoneLog(stderr, "error", __VA_ARGS__)
#define ozoneLogWarn(...) ozoneLog(stderr, "warn", __VA_ARGS__)
#define ozoneLogInfo(...) ozoneLog(stdout, "info", __VA_ARGS__)

#if defined(OZONE_LOG_TRACE) || defined(OZONE_LOG_DEBUG)
#define ozoneLogDebug(...) ozoneLog(stdout, "debug", __VA_ARGS__)
#else
#define ozoneLogDebug(...)                                                                                             \
  do {                                                                                                                 \
  } while (0)
#endif

#ifdef OZONE_LOG_TRACE
#define ozoneLogTrace(...) ozoneLog(stdout, "trace", __VA_ARGS__)
#else
#define ozoneLogTrace(...)                                                                                             \
  do {                                                                                                                 \
  } while (0)
#endif

#endif
