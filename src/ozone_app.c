#include "ozone_app.h"

#include "ozone_file.h"
#include "ozone_log.h"
#include <pthread.h>

#define OZONE_APP_HELP                                                                                                 \
  "ozone (pre-alpha) application help menu.\n\n"                                                                       \
  "Options:\n"                                                                                                         \
  "  --help                 Show this screen.\n"                                                                       \
  "  --max-workers=<value>  Max worker count for your application, 0 for automatic [default: 0].\n"                    \
  "  --port=<value>         Port for your application [default: 8080].\n"                                              \
  "  --<other-key>=<value>  Intended for custom application configuration, inserts 'option:<other-key>' into "         \
  "the OzoneAppEvent.context cache.\n"

#define OZONE_APP_DEFAULT_PORT 8080
#define OZONE_APP_DEFAULT_MAX_WORKERS 0 // automatic
#define OZONE_APP_MAX_OPTION_LENGTH 128

#define OZONE_APP_ROUTER_MATCH_NOT_FOUND 0
#define OZONE_APP_ROUTER_MATCH_SEEK 1
#define OZONE_APP_ROUTER_MATCH_WILDCARD_START 2
#define OZONE_APP_ROUTER_MATCH_WILDCARD_STOP 3

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)
OZONE_VECTOR_IMPLEMENT_API(OzoneAppVoidRef)
OZONE_MAP_IMPLEMENT_API(OzoneAppVoidRef)

int ozoneAppBeginPipeline(OzoneAppEvent* event) {
  ozoneLogTrace(
      "Beginning app pipeline, %ld live bytes in event allocator", ozoneAllocatorGetTotalFree(event->allocator));

  OzoneAppEndpoint* endpoint;
  ozoneVectorForEach(endpoint, event->context->endpoints) {
    if (event->request->method != endpoint->method)
      continue;

    int parsing = OZONE_APP_ROUTER_MATCH_SEEK;
    char wildcard_stop = '\0';
    OzoneString* parameter_name = NULL;
    OzoneString* parameter_value = NULL;

    size_t pattern_index = 0;
    size_t target_index = 0;
    while (parsing != OZONE_APP_ROUTER_MATCH_NOT_FOUND && target_index < ozoneStringLength(&event->request->target)) {

      char target_cursor = ozoneStringBufferAt(&event->request->target, target_index);
      char pattern_cursor = pattern_index < ozoneStringLength(&endpoint->target_pattern)
          ? ozoneStringBufferAt(&endpoint->target_pattern, pattern_index)
          : '\0';

      switch (parsing) {
      case OZONE_APP_ROUTER_MATCH_SEEK: {
        if (pattern_cursor == '{') {
          parsing = OZONE_APP_ROUTER_MATCH_WILDCARD_START;
          pattern_index++;
          parameter_name = ozoneStringAllocate(event->allocator, "path:");
        } else if (pattern_cursor == target_cursor) {
          pattern_index++;
          target_index++;
        } else {
          pattern_index = -1;
          target_index = -1;
        }
        break;
      }
      case OZONE_APP_ROUTER_MATCH_WILDCARD_START: {
        if (pattern_cursor == '}') {
          parsing = OZONE_APP_ROUTER_MATCH_WILDCARD_STOP;
          pattern_index++;
          parameter_value = ozoneStringAllocate(event->allocator, "");
        } else {
          ozoneStringWriteByte(event->allocator, parameter_name, pattern_cursor);
          pattern_index++;
        }
        break;
      }
      case OZONE_APP_ROUTER_MATCH_WILDCARD_STOP: {
        if (!wildcard_stop)
          wildcard_stop = pattern_cursor;

        if (target_cursor == wildcard_stop || target_cursor == '/') {
          wildcard_stop = '\0';
          parsing = OZONE_APP_ROUTER_MATCH_SEEK;
          OzoneStringMapInsert(event->allocator, &event->parameters, parameter_name, parameter_value);
          ozoneStringClear(parameter_name);
          parameter_value = NULL;
        } else {
          ozoneStringWriteByte(event->allocator, parameter_value, target_cursor);
          target_index++;
        }

        break;
      }
      }
    }

    if (pattern_index == ozoneStringLength(&endpoint->target_pattern)
        && target_index == ozoneStringLength(&event->request->target)) {
      if (parameter_name && parameter_value && ozoneStringLength(parameter_name)
          && ozoneStringLength(parameter_value)) {
        OzoneStringMapInsert(event->allocator, &event->parameters, parameter_name, parameter_value);
      }

      OzoneSocketHandlerRef* handler;
      ozoneVectorForEach(handler, &endpoint->handler_pipeline) { (*handler)((OzoneSocketEvent*)event); }
      return 0;
    }
  }

  event->response->code = 404;
  return 0;
}

int ozoneAppServe(int argc, char* argv[], OzoneAppEndpointVector* endpoints) {
  OzoneAppContext context = (OzoneAppContext) {
    .allocator = ozoneAllocatorCreate(4096),
    .endpoints = endpoints,
  };
  context.cache_lock = ozoneAllocatorReserveOne(context.allocator, pthread_mutex_t);
  context.cache = ozoneAllocatorReserveOne(context.allocator, OzoneAppVoidRefMap);

  int help = 0;
  unsigned int max_workers = OZONE_APP_DEFAULT_MAX_WORKERS;
  unsigned int port = OZONE_APP_DEFAULT_PORT;

  for (int option = 1; option < argc; option++) {
    OzoneString* option_key_value = ozoneStringFromBuffer(context.allocator, argv[option], OZONE_APP_MAX_OPTION_LENGTH);

    int option_marker = ozoneStringFindFirst(option_key_value, &ozoneString("--"));
    if (option_marker != 0) {
      ozoneLogError("Command line options should begin with '--'");
      help = 1;
      break;
    }

    OzoneString* value = NULL;
    int equals_at = ozoneStringFindFirst(option_key_value, &ozoneString("="));
    if (equals_at > 1)
      value = ozoneStringSlice(context.allocator, option_key_value, equals_at + 1, ozoneStringLength(option_key_value));

    OzoneString* key = ozoneStringSlice(
        context.allocator,
        option_key_value,
        2,
        equals_at > 1 ? (size_t)equals_at : ozoneStringLength(option_key_value));

    if (!ozoneStringCompare(key, &ozoneString("help"))) {
      help = 1;
      break;
    } else if (!ozoneStringCompare(key, &ozoneString("max-workers")) && value) {
      max_workers = (unsigned int)ozoneStringToInteger(value);
      ozoneLogInfo("Set max-workers to %d", max_workers);
    } else if (!ozoneStringCompare(key, &ozoneString("port")) && value) {
      port = (unsigned int)ozoneStringToInteger(value);
      ozoneLogInfo("Set port to %d", port);
    } else if (!value) {
      ozoneLogWarn("Ignored option '%s'", ozoneStringBuffer(key));
      help = 1;
      break;
    } else {
      // Cache ignored options in the event context
      OzoneString* cache_key = ozoneStringAllocate(context.allocator, "option:");
      ozoneStringConcatenate(context.allocator, cache_key, key);
      OzoneAppVoidRefMapInsert(context.allocator, context.cache, cache_key, (OzoneAppVoidRef*)&value);
      ozoneLogInfo("Added '%s' to event context cache", ozoneStringBuffer(cache_key));
    }
  }

  if (help) {
    printf(OZONE_APP_HELP);

    ozoneAllocatorDelete(context.allocator);
    return 0;
  }
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wcast-function-type"
#else
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif
  OzoneHTTPConfig http_config = (OzoneHTTPConfig) {
    .handler_context = &context,
    .handler_pipeline = ozoneVector(OzoneSocketHandlerRef, (OzoneSocketHandlerRef)ozoneAppBeginPipeline),
    .max_workers = max_workers,
    .port = port,
  };
#ifndef __clang__
#pragma GCC diagnostic pop
#else
#pragma clang diagnostic pop
#endif

  if (!endpoints) {
    ozoneLogError("Cannot serve an app with no endpoints");
    return 1;
  }

  pthread_mutex_init(context.cache_lock, NULL);
  int return_code = ozoneHTTPServe(&http_config);
  pthread_mutex_destroy(context.cache_lock);

  ozoneAllocatorDelete(context.allocator);

  return return_code;
}
