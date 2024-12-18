#ifndef OZONE_MACROS_API_H
#define OZONE_MACROS_API_H

#include "ozone_app.h"

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
#define ozoneHandler(_handler_name_, _handler_body_)                                                                   \
  int _handler_name_(OzoneAppContextT* handler_context)                                                                \
  {                                                                                                                    \
    OzoneAllocatorT* allocator = handler_context->allocator;                                                           \
    OzoneHTTPRequestT* req = &handler_context->parsed->request;                                                        \
    OzoneHTTPResponseT* res = &handler_context->parsed->response;                                                      \
    (void)allocator;                                                                                                   \
    (void)req;                                                                                                         \
    (void)res;                                                                                                         \
    _handler_body_ return 0;                                                                                           \
  }                                                                                                                    \
  int _handler_name_(OzoneAppContextT* handler_context)

#endif
