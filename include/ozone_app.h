#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_http.h"
#include "ozone_map.h"
#include "ozone_socket.h"
#include "ozone_string.h"
#include "ozone_template.h"
#include <pthread.h>

typedef struct OzoneAppEndpointStruct {
  OzoneHTTPMethod method;
  OzoneString target_pattern;
  OzoneSocketHandlerRefVector handler_pipeline;
} OzoneAppEndpoint;

OZONE_VECTOR_DECLARE_API(OzoneAppEndpoint)

typedef uintptr_t OzoneAppVoidRef;
OZONE_VECTOR_DECLARE_API(OzoneAppVoidRef)
OZONE_MAP_DECLARE_API(OzoneAppVoidRef)

typedef struct OzoneAppContextStruct {
  OzoneAllocator* allocator;
  OzoneAppEndpointVector* endpoints;
  pthread_mutex_t* cache_lock;
  OzoneAppVoidRefMap* cache;
} OzoneAppContext;

typedef struct OzoneAppEventStruct
    OZONE_SOCKET_EVENT_FIELDS(OzoneHTTPRequest, OzoneHTTPResponse, OzoneAppContext) OzoneAppEvent;

typedef void(OzoneAppHandler)(OzoneAppEvent* event);

#define ozoneAppEndpoint(_method_, _path_, ...)                                                                        \
  (OzoneAppEndpoint) {                                                                                                 \
    .method = OZONE_HTTP_METHOD_##_method_, .target_pattern = ozoneString(_path_),                                     \
    .handler_pipeline = ozoneVectorFromArray(OzoneSocketHandlerRef, ((OzoneAppHandler*[]) { __VA_ARGS__ }))            \
  }

int ozoneAppServe(int argc, char* argv[], OzoneAppEndpointVector* endpoints);

/* OzoneAppEvent convenience functions */

#define ozoneAppContextLock(_event_) pthread_mutex_lock((_event_)->context->cache_lock);
#define ozoneAppContextUnlock(_event_) pthread_mutex_unlock((_event_)->context->cache_lock);
#define ozoneAppContextCacheGetRef(_event_, _type_, _cache_key_)                                                       \
  (_type_**)(OzoneAppVoidRefMapFind((_event_)->context->cache, &ozoneString(_cache_key_)));
#define ozoneAppContextCacheGetOrCreate(_event_, _type_, _cache_key_, _cache_value_, _create_if_not_cached_)           \
  do {                                                                                                                 \
    OzoneString cache_key = ozoneString(_cache_key_);                                                                  \
    OzoneAppVoidRef* cache_value = OzoneAppVoidRefMapFind((_event_)->context->cache, &cache_key);                      \
    if (cache_value) {                                                                                                 \
      _cache_value_ = (_type_*)(*cache_value);                                                                         \
    } else {                                                                                                           \
      do                                                                                                               \
        _create_if_not_cached_ while (0);                                                                              \
      OzoneAppVoidRefMapInsert(                                                                                        \
          (_event_)->context->allocator, (_event_)->context->cache, &cache_key, (OzoneAppVoidRef*)&(_cache_value_));   \
    }                                                                                                                  \
  } while (0)
#define ozoneAppParameter(_event_, _key_) OzoneStringMapFind(&event->parameters, &ozoneString(_key_))
#define ozoneAppRedirect(_event_, _location_)                                                                          \
  do {                                                                                                                 \
    (_event_)->response->code = 301;                                                                                   \
    OzoneStringMapInsert((_event_)->allocator, &(_event_)->response->headers, &ozoneString("Location"), _location_);   \
  } while (0)

#endif
