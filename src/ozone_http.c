#include "ozone_http.h"

#include "ozone_log.h"
#include <stdlib.h>
#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(HTTPHeaderT)

#define OZONE_REMAINING_CURSOR_SIZE (buffer + buffer_size - cursor)

OzoneHTTPVersionT ozoneHTTPParseVersion(const OzoneStringT* version_string) {
  if (ozoneStringCompare(version_string, &ozoneCharArray("HTTP/1.1")) == 0)
    return OZONE_HTTP_VERSION_1_1;
  if (ozoneStringCompare(version_string, &ozoneCharArray("HTTP/1.0")) == 0)
    return OZONE_HTTP_VERSION_1_0;

  ozoneLogWarn("Could not parse HTTP version");
  return OZONE_HTTP_VERSION_UNKNOWN;
}

OzoneHTTPMethodT ozoneHTTPParseMethod(const OzoneStringT* method_string) {
  if (ozoneStringCompare(method_string, &ozoneCharArray("GET")) == 0)
    return OZONE_HTTP_METHOD_GET;
  if (ozoneStringCompare(method_string, &ozoneCharArray("HEAD")) == 0)
    return OZONE_HTTP_METHOD_HEAD;
  if (ozoneStringCompare(method_string, &ozoneCharArray("POST")) == 0)
    return OZONE_HTTP_METHOD_POST;
  if (ozoneStringCompare(method_string, &ozoneCharArray("PUT")) == 0)
    return OZONE_HTTP_METHOD_PUT;
  if (ozoneStringCompare(method_string, &ozoneCharArray("DELETE")) == 0)
    return OZONE_HTTP_METHOD_DELETE;
  if (ozoneStringCompare(method_string, &ozoneCharArray("CONNECT")) == 0)
    return OZONE_HTTP_METHOD_CONNECT;
  if (ozoneStringCompare(method_string, &ozoneCharArray("OPTIONS")) == 0)
    return OZONE_HTTP_METHOD_OPTIONS;
  if (ozoneStringCompare(method_string, &ozoneCharArray("PATCH")) == 0)
    return OZONE_HTTP_METHOD_PATCH;
  if (ozoneStringCompare(method_string, &ozoneCharArray("TRACE")) == 0)
    return OZONE_HTTP_METHOD_TRACE;

  ozoneLogWarn("Could not parse HTTP method");
  return OZONE_HTTP_METHOD_UNKNOWN;
}

OzoneStringT ozoneHTTPStatusString(OzoneHTTPStatusCodeT status) {
  switch (status) {
  case 100:
    return ozoneCharArray("100 Continue");
  case 101:
    return ozoneCharArray("101 Switching Protocols");
  case 102:
    return ozoneCharArray("102 Processing");
  case 103:
    return ozoneCharArray("103 Early Hints");
  case 200:
    return ozoneCharArray("200 OK");
  case 201:
    return ozoneCharArray("201 Created");
  case 202:
    return ozoneCharArray("202 Accepted");
  case 203:
    return ozoneCharArray("203 Non-Authoritative Information");
  case 204:
    return ozoneCharArray("204 No Content");
  case 205:
    return ozoneCharArray("205 Reset Content");
  case 206:
    return ozoneCharArray("206 Partial Content");
  case 207:
    return ozoneCharArray("207 Multi-Status");
  case 208:
    return ozoneCharArray("208 Already Reported");
  case 226:
    return ozoneCharArray("226 IM Used");
  case 300:
    return ozoneCharArray("300 Multiple Choices");
  case 301:
    return ozoneCharArray("301 Moved Permanently");
  case 302:
    return ozoneCharArray("302 Found");
  case 303:
    return ozoneCharArray("303 See Other");
  case 304:
    return ozoneCharArray("304 Not Modified");
  case 305:
    return ozoneCharArray("305 Use Proxy");
  case 307:
    return ozoneCharArray("307 Temporary Redirect");
  case 308:
    return ozoneCharArray("308 Permanent Redirect");
  case 400:
    return ozoneCharArray("400 Bad Request");
  case 401:
    return ozoneCharArray("401 Unauthorized");
  case 402:
    return ozoneCharArray("402 Payment Required");
  case 403:
    return ozoneCharArray("403 Forbidden");
  case 404:
    return ozoneCharArray("404 Not Found");
  case 405:
    return ozoneCharArray("405 Method Not Allowed");
  case 406:
    return ozoneCharArray("406 Not Acceptable");
  case 407:
    return ozoneCharArray("407 Proxy Authentication Required");
  case 408:
    return ozoneCharArray("408 Request Timeout");
  case 409:
    return ozoneCharArray("409 Conflict");
  case 410:
    return ozoneCharArray("410 Gone");
  case 411:
    return ozoneCharArray("411 Length Required");
  case 412:
    return ozoneCharArray("412 Precondition Failed");
  case 413:
    return ozoneCharArray("413 Content Too Large");
  case 414:
    return ozoneCharArray("414 URI Too Long");
  case 415:
    return ozoneCharArray("415 Unsupported Media Type");
  case 416:
    return ozoneCharArray("416 Range Not Satisfiable");
  case 417:
    return ozoneCharArray("417 Expectation Failed");
  case 418:
    return ozoneCharArray("418 I'm a teapot");
  case 421:
    return ozoneCharArray("421 Misdirected Request");
  case 422:
    return ozoneCharArray("422 Unprocessable Content");
  case 423:
    return ozoneCharArray("423 Locked");
  case 424:
    return ozoneCharArray("424 Failed Dependency");
  case 425:
    return ozoneCharArray("425 Too Early");
  case 426:
    return ozoneCharArray("426 Upgrade Required");
  case 428:
    return ozoneCharArray("428 Precondition Required");
  case 429:
    return ozoneCharArray("429 Too Many Requests");
  case 431:
    return ozoneCharArray("431 Request Header Fields Too Large");
  case 500:
    return ozoneCharArray("500 Internal Server Error");
  case 501:
    return ozoneCharArray("501 Not Implemented");
  case 502:
    return ozoneCharArray("502 Bad Gateway");
  case 503:
    return ozoneCharArray("503 Service Unavailable");
  case 504:
    return ozoneCharArray("504 Gateway Timeout");
  case 505:
    return ozoneCharArray("505 HTTP Version Not Supported");
  case 506:
    return ozoneCharArray("506 Variant Also Negotiates");
  case 507:
    return ozoneCharArray("507 Insufficient Storage");
  case 508:
    return ozoneCharArray("508 Loop Detected");
  case 510:
    return ozoneCharArray("510 Not Extended");
  case 511:
    return ozoneCharArray("511 Network Authentication Required");
  default:
    ozoneLogWarn("Unknown HTTP status %d", status);
    return ozoneCharArray("500 Internal Server Error");
  }
}

OzoneStringT* ozoneHTTPGetHeaderValue(OzoneHTTPHeaderTVectorT* headers, OzoneStringT name) {
  for (size_t header_index = 0; header_index < headers->length; header_index++) {
    OzoneHTTPHeaderT* header = &headers->elements[header_index];
    if (ozoneStringCompare(&header->name, &name) == 0)
      return &header->value;
  }

  return NULL;
}

OzoneHTTPRequestT* ozoneHTTPParseSocketChunks(OzoneAllocatorT* allocator, const OzoneSocketChunkT* socket_request) {
  size_t buffer_size = 0;

  // todo: extract to helper
  const OzoneSocketChunkT* chunk = socket_request;
  do {
    buffer_size += chunk->length;
  } while ((chunk = chunk->next));
  char* buffer = ozoneAllocatorReserveMany(allocator, char, buffer_size);

  char* cursor = buffer;
  chunk = socket_request;
  do {
    memcpy(cursor, chunk->buffer, chunk->length);
    cursor += chunk->length;
  } while ((chunk = chunk->next));
  cursor = buffer;
  // end todo

  OzoneHTTPRequestT* http_request = ozoneAllocatorReserveOne(allocator, OzoneHTTPRequestT);
  *http_request = (OzoneHTTPRequestT) { 0 };

  OzoneStringT* method_string
      = ozoneCharArrayScanBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneCharArray(" "));
  if (OZONE_HTTP_METHOD_UNKNOWN == (http_request->method = ozoneHTTPParseMethod(method_string)))
    return NULL;
  cursor += method_string->length;

  http_request->target
      = *ozoneCharArrayScanBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneCharArray(" "));
  cursor += http_request->target.length;

  OzoneStringT* http_version_string
      = ozoneCharArrayScanBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneCharArray("\r\n"));
  if (OZONE_HTTP_VERSION_UNKNOWN == (http_request->version = ozoneHTTPParseVersion(http_version_string)))
    return NULL;
  cursor += http_version_string->length + 1;

  while (buffer + buffer_size > (cursor + 1) && cursor[0] != '\r') {
    OzoneStringT* name = ozoneCharArrayScanBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneCharArray(":"));
    if (!name)
      break;

    cursor += name->length + 1;

    OzoneStringT* value
        = ozoneCharArrayScanBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneCharArray("\r\n"));
    if (!value)
      break;

    ozoneHTTPAppendHeader(allocator, &http_request->headers, *name, *value);
    cursor += value->length + 1;
  }

  cursor += 2;

  OzoneStringT* content_length_string
      = ozoneHTTPGetHeaderValue(&http_request->headers, ozoneCharArray("Content-Length"));
  if (!content_length_string)
    return http_request;

  long int content_length = strtol(content_length_string->buffer, NULL, 10);
  if (content_length == 0)
    return http_request;

  if (content_length > OZONE_REMAINING_CURSOR_SIZE) {
    ozoneLogWarn(
        "HTTP request specified a Content-Length of %ld but only %ld bytes remain in the buffer, dropping request body",
        content_length, OZONE_REMAINING_CURSOR_SIZE);
    return http_request;
  }

  cursor[content_length + 1] = '\0';
  http_request->body
      = (OzoneStringT) { .buffer = cursor, .length = content_length + 1, .encoding = OZONE_STRING_ENCODING_ISO_8859_1 };

  return http_request;
}

OzoneSocketChunkT* ozoneHTTPCreateSocketChunks(OzoneAllocatorT* allocator, OzoneHTTPResponseT* http_response) {
  // todo: HTTP versioned response handling; upgrading and downgrading
  OzoneStringT version = ozoneCharArray("HTTP/1.0");
  OzoneStringT status = ozoneHTTPStatusString(http_response->code);

  size_t buffer_size = version.length + status.length + 1;

  for (size_t header_index = 0; header_index < http_response->headers.length; header_index++) {
    OzoneHTTPHeaderT* header = &http_response->headers.elements[header_index];
    buffer_size += header->name.length + 1 + header->value.length + 1;
  }

  buffer_size += 2;

  if (http_response->body.length)
    buffer_size += http_response->body.length - 1;

  OzoneSocketChunkT* socket_chunk = ozoneAllocatorReserveOne(allocator, OzoneSocketChunkT);
  socket_chunk->buffer = ozoneAllocatorReserveMany(allocator, char, buffer_size);
  socket_chunk->length = buffer_size;

  char* cursor = socket_chunk->buffer;
  memcpy(cursor, version.buffer, version.length - 1);
  cursor[version.length - 1] = ' ';
  cursor += version.length;
  memcpy(cursor, status.buffer, status.length - 1);
  cursor[status.length - 1] = '\r';
  cursor[status.length] = '\n';
  cursor += status.length + 1;

  for (size_t header_index = 0; header_index < http_response->headers.length; header_index++) {
    OzoneHTTPHeaderT* header = &http_response->headers.elements[header_index];
    memcpy(cursor, header->name.buffer, header->name.length - 1);
    cursor[header->name.length - 1] = ':';
    cursor[header->name.length] = ' ';
    cursor += header->name.length + 1;
    memcpy(cursor, header->value.buffer, header->value.length - 1);
    cursor[header->value.length - 1] = '\r';
    cursor[header->value.length] = '\n';
    cursor += header->value.length + 1;
  }

  cursor[0] = '\r';
  cursor[1] = '\n';
  cursor += 2;

  if (http_response->body.length) {
    memcpy(cursor, http_response->body.buffer, http_response->body.length - 1);
  }

  return socket_chunk;
}

int ozoneHTTPBeginPipeline(OzoneHTTPContextT* context) {
  context->parsed_request = ozoneHTTPParseSocketChunks(context->allocator, context->raw_request);
  context->parsed_response = ozoneAllocatorReserveOne(context->allocator, OzoneHTTPResponseT);
  *context->parsed_response = (OzoneHTTPResponseT) { 0 };

  ozoneLogInfo("%s",
      ozoneStringScanBuffer(context->allocator, context->raw_request->buffer, context->raw_request->length,
          &ozoneCharArray("\r"), OZONE_STRING_ENCODING_ISO_8859_1)
          ->buffer);

  return 0;
}

int ozoneHTTPEndPipeline(OzoneHTTPContextT* context) {
  if (!context->parsed_response)
    return 0;

  OzoneHTTPResponseT* response = context->parsed_response;

  if (!response->code)
    response->code = response->body.length ? 200 : 204;

  if (response->body.length) {
    if (!ozoneHTTPGetHeaderValue(&response->headers, ozoneCharArray("Content-Type")))
      ozoneHTTPAppendHeader(
          context->allocator, &response->headers, ozoneCharArray("Content-Type"), ozoneCharArray("text/plain"));

    if (!ozoneHTTPGetHeaderValue(&response->headers, ozoneCharArray("Content-Length"))) {
      // todo: extract to helper
      char content_length[32] = { 0 };
      snprintf(content_length, sizeof(content_length), "%ld", response->body.length - 1);

      size_t length = 0;
      while (content_length[length++] && length < sizeof(content_length))
        ;
      // end todo

      ozoneHTTPHeaderTVectorPush(context->allocator, &response->headers,
          (OzoneHTTPHeaderT) {
              .name = ozoneCharArray("Content-Length"),
              .value = (OzoneStringT) { .buffer = content_length,
                  .length = length,
                  .encoding = OZONE_STRING_ENCODING_ISO_8859_1,
             },
          });
    }
  }

  context->raw_response = ozoneHTTPCreateSocketChunks(context->allocator, response);
  ozoneLogInfo("%s",
      ozoneStringScanBuffer(context->allocator, context->raw_response->buffer, context->raw_response->length,
          &ozoneCharArray("\r"), OZONE_STRING_ENCODING_ISO_8859_1)
          ->buffer);

  return 0;
}

int ozoneHTTPServe(OzoneHTTPConfigT config) {
  size_t http_pipeline_count = 2 + config.handler_pipeline_count;
  OzoneSocketHandlerT** http_pipeline
      = ozoneAllocatorReserveMany(config.allocator, OzoneSocketHandlerT*, http_pipeline_count);
  http_pipeline[0] = (OzoneSocketHandlerT*)ozoneHTTPBeginPipeline;
  for (size_t handler_index = 0; handler_index < config.handler_pipeline_count; handler_index++) {
    http_pipeline[handler_index + 1] = (OzoneSocketHandlerT*)config.handler_pipeline[handler_index];
  }
  http_pipeline[http_pipeline_count - 1] = (OzoneSocketHandlerT*)ozoneHTTPEndPipeline;

  ozoneLogInfo("Serving at http://localhost:%d", config.port);
  return ozoneSocketServeTCP((OzoneSocketConfigT) {
      .handler_pipeline = http_pipeline,
      .handler_pipeline_count = http_pipeline_count,
      .port = config.port,
      .application = config.application,
  });
}
