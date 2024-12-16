#ifndef OZONE_MACROS_API_H
#define OZONE_MACROS_API_H

#include "ozone_http.h"

/**
 * \brief Define an ozone HTTP handler
 *
 * The following parameters are provided for convenience:
 *
 * \param OzoneAllocatorT* allocator
 * \param OzoneHTTPHandlerContextT* http
 * \param OzoneHTTPRequestT* req
 * \param OzoneHTTPResponseT* res
 *
 * \returns int 0 on success, or errno on failure
 */
#define ozoneHTTPHandler(_handler_name_, _handler_body_)                                                               \
  int _handler_name_(OzoneHTTPSocketHandlerContextT* _handler_context)                                                 \
  {                                                                                                                    \
    OzoneAllocatorT* allocator = _handler_context->allocator;                                                          \
    OzoneHTTPHandlerContextT* http = _handler_context->extra_context;                                                  \
    OzoneHTTPRequestT* req = &http->request;                                                                           \
    OzoneHTTPResponseT* res = &http->response;                                                                         \
    (void)allocator;                                                                                                   \
    (void)req;                                                                                                         \
    (void)res;                                                                                                         \
    _handler_body_ return 0;                                                                                           \
  }                                                                                                                    \
  int _handler_name_(OzoneHTTPSocketHandlerContextT* _handler_context)

#endif
