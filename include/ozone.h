#ifndef OZONE_H
#define OZONE_H

#include "ozone_allocator.h"
#include "ozone_array.h"
#include "ozone_http.h"
#include "ozone_log.h"
#include "ozone_socket.h"

/**
 * Define an ozone HTTP handler
 *
 * @param OzoneHTTPRequestT* req
 * @param OzoneHTTPResponseT* res
 * @param int err = 0;
 */
#define ozoneHTTPHandler(handler_name, handler_body)           \
  int handler_name(OzoneHTTPHandlerContextT *_handler_context) \
  {                                                            \
    OzoneHTTPRequestT *req = _handler_context->request;        \
    OzoneHTTPResponseT *res = _handler_context->response;      \
    int err = 0;                                               \
    (void)req;                                                 \
    (void)res;                                                 \
    handler_body return err;                                   \
  }                                                            \
  int handler_name(OzoneHTTPHandlerContextT *http)

#endif
