#include "ozone_app.h"

#include "ozone_file.h"
#include "ozone_log.h"
#include <pthread.h>

#define OZONE_APP_HELP                                                                                                 \
  "ozone (pre-alpha) application help menu.\n\n"                                                                       \
  "Options:\n"                                                                                                         \
  "  --help                 Show this screen.\n"                                                                       \
  "  --port=<port>          Port for your application [default: 8080].\n"                                              \
  "  --<other-key>=<value>  Intended for custom application configuration, inserts 'option:<other-key>' into "         \
  "the OzoneAppEvent.context cache.\n"

#define OZONE_APP_DEFAULT_PORT 8080
#define OZONE_APP_MAX_OPTION_LENGTH 128

OZONE_VECTOR_IMPLEMENT_API(OzoneAppEndpoint)

OZONE_VECTOR_IMPLEMENT_API(OzoneAppVoidRef)
OZONE_MAP_IMPLEMENT_API(OzoneAppVoidRef)

void ozoneAppSetResponseHeader(OzoneAppEvent* event, const OzoneString* name, const OzoneString* value) {
  ozoneMapInsertOzoneString(
      event->allocator, &event->response->headers, name, ozoneStringCopy(event->allocator, value));
}

int ozoneAppBeginPipeline(OzoneAppEvent* event) {
  ozoneLogTrace(
      "Beginning app pipeline, %ld live bytes in event allocator", ozoneAllocatorGetTotalFree(event->allocator));

  OzoneAppEndpoint* endpoint;
  ozoneVectorForEach(endpoint, &event->context->endpoints) {
    if (event->request->method != endpoint->method)
      continue;

    if (ozoneStringCompare(&event->request->target, &endpoint->target_pattern) != 0)
      continue;

    OzoneSocketHandlerRef* handler;
    ozoneVectorForEach(handler, &endpoint->handler_pipeline) { (*handler)((OzoneSocketEvent*)event); }
    return 0;
  }

  event->response->code = 404;
  return 0;
}

int ozoneAppServe(int argc, char* argv[], OzoneAppEndpointVector* endpoints) {
  OzoneAppContext context = (OzoneAppContext) { .allocator = ozoneAllocatorCreate(4096), .endpoints = *endpoints };
  context.cache_lock = ozoneAllocatorReserveOne(context.allocator, pthread_mutex_t);
  context.cache = ozoneAllocatorReserveOne(context.allocator, OzoneAppVoidRefMap);

  int help = 0;
  unsigned short port = OZONE_APP_DEFAULT_PORT;

  for (int option = 1; option < argc; option++) {
    OzoneString* option_key_value = ozoneStringFromBuffer(context.allocator, argv[option], OZONE_APP_MAX_OPTION_LENGTH);

    int option_marker = ozoneStringFindFirst(option_key_value, &ozoneStringConstant("--"));
    if (option_marker != 0) {
      ozoneLogError("Command line options should begin with '--'");
      help = 1;
      break;
    }

    OzoneString* value = NULL;
    int equals_at = ozoneStringFindFirst(option_key_value, &ozoneStringConstant("="));
    if (equals_at > 1)
      value = ozoneStringSlice(context.allocator, option_key_value, equals_at + 1, ozoneStringLength(option_key_value));

    OzoneString* key = ozoneStringSlice(
        context.allocator, option_key_value, 2, equals_at > 1 ? equals_at : ozoneStringLength(option_key_value));

    if (!ozoneStringCompare(key, &ozoneStringConstant("help"))) {
      help = 1;
      break;
    } else if (!ozoneStringCompare(key, &ozoneStringConstant("port")) && value) {
      port = (unsigned short)ozoneStringToInteger(value);
    } else if (!value) {
      ozoneLogWarn("Ignored option '%s'", ozoneStringBuffer(key));
      help = 1;
      break;
    } else {
      // Cache ignored options in the event context
      OzoneString* cache_key = ozoneString(context.allocator, "option:");
      ozoneStringConcatenate(context.allocator, cache_key, key);
      ozoneMapInsertOzoneAppVoidRef(context.allocator, context.cache, cache_key, (OzoneAppVoidRef*)&value);
      ozoneLogDebug("Added '%s' to event context cache", ozoneStringBuffer(cache_key));
    }
  }

  if (help) {
    printf(OZONE_APP_HELP);

    ozoneAllocatorDelete(context.allocator);
    return 0;
  }

  OzoneHTTPConfig http_config = (OzoneHTTPConfig) {
    .port = port,
    .handler_pipeline = ozoneVectorFromElements(OzoneSocketHandlerRef, (OzoneSocketHandlerRef)ozoneAppBeginPipeline),
    .handler_context = &context
  };

  pthread_mutex_init(context.cache_lock, NULL);
  int return_code = ozoneHTTPServe(&http_config);
  pthread_mutex_destroy(context.cache_lock);

  ozoneAllocatorDelete(context.allocator);

  return return_code;
}
