#ifndef OZONE_MACROS_API_H
#define OZONE_MACROS_API_H

#include "ozone_app.h"

#define ozoneHTTPEndpoint(_method_, _target_pattern_, _handler_pipeline_)                                              \
  (OzoneRouterHTTPEndpoint) {                                                                                          \
    .config = (OzoneRouterHTTPConfig) { .method = OZONE_HTTP_METHOD_##_method_,                                        \
      .target_pattern = ozoneString(_target_pattern_) },                                                               \
    .handler_pipeline = _handler_pipeline_,                                                                            \
    .handler_pipeline_count = sizeof(_handler_pipeline_) / sizeof(OzoneHTTPHandler*)                                   \
  }

/**
 * \brief Define an ozone HTTP handler
 *
 * The following parameters are provided for convenience:
 *
 * \param OzoneHTTPContext* ctx
 * \param OzoneAllocator* allocator
 * \param OzoneHTTPRequest* req
 * \param OzoneHTTPResponse* res
 *
 * \returns int 0 on success, or errno on failure
 */
#define ozoneHTTPHandler(_handler_name_, _handler_body_)                                                               \
  int _handler_name_(OzoneHTTPContext* ctx) {                                                                          \
    OzoneAllocator* allocator = ctx->allocator;                                                                        \
    OzoneHTTPRequest* req = ctx->parsed_request;                                                                       \
    OzoneHTTPResponse* res = ctx->parsed_response;                                                                     \
    (void)allocator;                                                                                                   \
    (void)req;                                                                                                         \
    (void)res;                                                                                                         \
    _handler_body_ return 0;                                                                                           \
  }                                                                                                                    \
  int _handler_name_(OzoneHTTPContext* ctx)

#endif
