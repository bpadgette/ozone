#ifndef OZONE_MACROS_API_H
#define OZONE_MACROS_API_H

#include "ozone_app.h"

#define ozoneHTTPEndpoint(_method_, _target_pattern_, _handler_pipeline_)                                              \
  (OzoneRouterHTTPEndpointT) {                                                                                         \
    .config = (OzoneRouterHTTPConfigT) { .method = OZONE_HTTP_METHOD_##_method_,                                       \
      .target_pattern = ozoneCharArray(_target_pattern_) },                                                            \
    .handler_pipeline = _handler_pipeline_,                                                                            \
    .handler_pipeline_count = sizeof(_handler_pipeline_) / sizeof(OzoneHTTPHandlerT*)                                  \
  }

/**
 * \brief Define an ozone HTTP handler
 *
 * The following parameters are provided for convenience:
 *
 * \param OzoneHTTPContextT* ctx
 * \param OzoneAllocatorT* allocator
 * \param OzoneHTTPRequestT* req
 * \param OzoneHTTPResponseT* res
 *
 * \returns int 0 on success, or errno on failure
 */
#define ozoneHTTPHandler(_handler_name_, _handler_body_)                                                               \
  int _handler_name_(OzoneHTTPContextT* ctx) {                                                                         \
    OzoneAllocatorT* allocator = ctx->allocator;                                                                       \
    OzoneHTTPRequestT* req = ctx->parsed_request;                                                                      \
    OzoneHTTPResponseT* res = ctx->parsed_response;                                                                    \
    (void)allocator;                                                                                                   \
    (void)req;                                                                                                         \
    (void)res;                                                                                                         \
    _handler_body_ return 0;                                                                                           \
  }                                                                                                                    \
  int _handler_name_(OzoneHTTPContextT* ctx)

#endif
