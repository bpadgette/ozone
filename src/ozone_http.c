#include "ozone_http.h"

#include "ozone_log.h"
#include <stdlib.h>
#include <string.h>

OZONE_VECTOR_IMPLEMENT_API(OzoneHTTPHeaderT)

#define OZONE_REMAINING_CURSOR_SIZE (buffer + buffer_size - cursor)

OzoneHTTPVersionT ozoneHTTPParseVersion(const OzoneStringT* version_string) {
  if (ozoneStringCompare(version_string, &ozoneString("HTTP/1.1")) == 0)
    return OZONE_HTTP_VERSION_1_1;
  if (ozoneStringCompare(version_string, &ozoneString("HTTP/1.0")) == 0)
    return OZONE_HTTP_VERSION_1_0;

  ozoneLogWarn("Could not parse HTTP version");
  return OZONE_HTTP_VERSION_UNKNOWN;
}

OzoneHTTPMethodT ozoneHTTPParseMethod(const OzoneStringT* method_string) {
  if (ozoneStringCompare(method_string, &ozoneString("GET")) == 0)
    return OZONE_HTTP_METHOD_GET;
  if (ozoneStringCompare(method_string, &ozoneString("HEAD")) == 0)
    return OZONE_HTTP_METHOD_HEAD;
  if (ozoneStringCompare(method_string, &ozoneString("POST")) == 0)
    return OZONE_HTTP_METHOD_POST;
  if (ozoneStringCompare(method_string, &ozoneString("PUT")) == 0)
    return OZONE_HTTP_METHOD_PUT;
  if (ozoneStringCompare(method_string, &ozoneString("DELETE")) == 0)
    return OZONE_HTTP_METHOD_DELETE;
  if (ozoneStringCompare(method_string, &ozoneString("CONNECT")) == 0)
    return OZONE_HTTP_METHOD_CONNECT;
  if (ozoneStringCompare(method_string, &ozoneString("OPTIONS")) == 0)
    return OZONE_HTTP_METHOD_OPTIONS;
  if (ozoneStringCompare(method_string, &ozoneString("PATCH")) == 0)
    return OZONE_HTTP_METHOD_PATCH;
  if (ozoneStringCompare(method_string, &ozoneString("TRACE")) == 0)
    return OZONE_HTTP_METHOD_TRACE;

  ozoneLogWarn("Could not parse HTTP method");
  return OZONE_HTTP_METHOD_UNKNOWN;
}

OzoneStringT ozoneHTTPStatusString(OzoneHTTPStatusCodeT status) {
  switch (status) {
  case 100:
    return ozoneString("100 Continue");
  case 101:
    return ozoneString("101 Switching Protocols");
  case 102:
    return ozoneString("102 Processing");
  case 103:
    return ozoneString("103 Early Hints");
  case 200:
    return ozoneString("200 OK");
  case 201:
    return ozoneString("201 Created");
  case 202:
    return ozoneString("202 Accepted");
  case 203:
    return ozoneString("203 Non-Authoritative Information");
  case 204:
    return ozoneString("204 No Content");
  case 205:
    return ozoneString("205 Reset Content");
  case 206:
    return ozoneString("206 Partial Content");
  case 207:
    return ozoneString("207 Multi-Status");
  case 208:
    return ozoneString("208 Already Reported");
  case 226:
    return ozoneString("226 IM Used");
  case 300:
    return ozoneString("300 Multiple Choices");
  case 301:
    return ozoneString("301 Moved Permanently");
  case 302:
    return ozoneString("302 Found");
  case 303:
    return ozoneString("303 See Other");
  case 304:
    return ozoneString("304 Not Modified");
  case 305:
    return ozoneString("305 Use Proxy");
  case 307:
    return ozoneString("307 Temporary Redirect");
  case 308:
    return ozoneString("308 Permanent Redirect");
  case 400:
    return ozoneString("400 Bad Request");
  case 401:
    return ozoneString("401 Unauthorized");
  case 402:
    return ozoneString("402 Payment Required");
  case 403:
    return ozoneString("403 Forbidden");
  case 404:
    return ozoneString("404 Not Found");
  case 405:
    return ozoneString("405 Method Not Allowed");
  case 406:
    return ozoneString("406 Not Acceptable");
  case 407:
    return ozoneString("407 Proxy Authentication Required");
  case 408:
    return ozoneString("408 Request Timeout");
  case 409:
    return ozoneString("409 Conflict");
  case 410:
    return ozoneString("410 Gone");
  case 411:
    return ozoneString("411 Length Required");
  case 412:
    return ozoneString("412 Precondition Failed");
  case 413:
    return ozoneString("413 Content Too Large");
  case 414:
    return ozoneString("414 URI Too Long");
  case 415:
    return ozoneString("415 Unsupported Media Type");
  case 416:
    return ozoneString("416 Range Not Satisfiable");
  case 417:
    return ozoneString("417 Expectation Failed");
  case 418:
    return ozoneString("418 I'm a teapot");
  case 421:
    return ozoneString("421 Misdirected Request");
  case 422:
    return ozoneString("422 Unprocessable Content");
  case 423:
    return ozoneString("423 Locked");
  case 424:
    return ozoneString("424 Failed Dependency");
  case 425:
    return ozoneString("425 Too Early");
  case 426:
    return ozoneString("426 Upgrade Required");
  case 428:
    return ozoneString("428 Precondition Required");
  case 429:
    return ozoneString("429 Too Many Requests");
  case 431:
    return ozoneString("431 Request Header Fields Too Large");
  case 500:
    return ozoneString("500 Internal Server Error");
  case 501:
    return ozoneString("501 Not Implemented");
  case 502:
    return ozoneString("502 Bad Gateway");
  case 503:
    return ozoneString("503 Service Unavailable");
  case 504:
    return ozoneString("504 Gateway Timeout");
  case 505:
    return ozoneString("505 HTTP Version Not Supported");
  case 506:
    return ozoneString("506 Variant Also Negotiates");
  case 507:
    return ozoneString("507 Insufficient Storage");
  case 508:
    return ozoneString("508 Loop Detected");
  case 510:
    return ozoneString("510 Not Extended");
  case 511:
    return ozoneString("511 Network Authentication Required");
  default:
    ozoneLogWarn("Unknown HTTP status %d", status);
    return ozoneString("500 Internal Server Error");
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

  OzoneStringT method_string = ozoneStringScanBuffer(
      allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString(" "), OZONE_STRING_ENCODING_ISO_8859_1);
  if (OZONE_HTTP_METHOD_UNKNOWN == (http_request->method = ozoneHTTPParseMethod(&method_string)))
    return NULL;
  cursor += ozoneStringLength(&method_string) + 1;

  http_request->target = ozoneStringScanBuffer(
      allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString(" "), OZONE_STRING_ENCODING_ISO_8859_1);
  cursor += ozoneStringLength(&http_request->target) + 1;

  OzoneStringT http_version_string = ozoneStringScanBuffer(
      allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString("\r\n"), OZONE_STRING_ENCODING_ISO_8859_1);
  if (OZONE_HTTP_VERSION_UNKNOWN == (http_request->version = ozoneHTTPParseVersion(&http_version_string)))
    return NULL;
  cursor += ozoneStringLength(&http_version_string) + 2;

  while (buffer + buffer_size > (cursor + 1) && cursor[0] != '\r') {
    OzoneStringT name = ozoneStringScanBuffer(
        allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString(":"), OZONE_STRING_ENCODING_ISO_8859_1);
    if (!ozoneStringLength(&name))
      break;

    cursor += ozoneStringLength(&name) + 2;

    OzoneStringT value = ozoneStringScanBuffer(
        allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString("\r\n"), OZONE_STRING_ENCODING_ISO_8859_1);
    if (!ozoneStringLength(&value))
      break;

    ozoneHTTPAppendHeader(allocator, &http_request->headers, &name, &value);
    cursor += ozoneStringLength(&value) + 2;
  }

  cursor += 2;

  OzoneStringT* content_length_string = ozoneHTTPGetHeaderValue(&http_request->headers, ozoneString("Content-Length"));
  if (!content_length_string)
    return http_request;

  long int content_length = strtol(ozoneStringBuffer(content_length_string), NULL, 10);
  if (content_length == 0)
    return http_request;

  if (content_length > OZONE_REMAINING_CURSOR_SIZE) {
    ozoneLogWarn(
        "HTTP request specified a Content-Length of %ld but only %ld bytes remain in the buffer, dropping request body",
        content_length, OZONE_REMAINING_CURSOR_SIZE);
    return http_request;
  }

  cursor[content_length + 1] = '\0';
  http_request->body = ((OzoneStringT) {
      .vector = ((OzoneVectorCharT) {
          .elements = cursor,
          .length = content_length + 1,
          .capacity = content_length + 1,
          .capacity_increment = content_length + 1,
      }),
      .encoding = OZONE_STRING_ENCODING_ISO_8859_1,
  });

  return http_request;
}

OzoneSocketChunkT* ozoneHTTPCreateSocketChunks(OzoneAllocatorT* allocator, OzoneHTTPResponseT* http_response) {
  // todo: HTTP versioned response handling; upgrading and downgrading
  OzoneStringT version = ozoneString("HTTP/1.0");
  OzoneStringT status = ozoneHTTPStatusString(http_response->code);

  size_t buffer_size = ozoneStringLength(&version) + ozoneStringLength(&status) + 3;

  for (size_t header_index = 0; header_index < http_response->headers.length; header_index++) {
    OzoneHTTPHeaderT* header = &http_response->headers.elements[header_index];
    buffer_size += ozoneStringLength(&header->name) + 2 + ozoneStringLength(&header->value) + 2;
  }

  buffer_size += 2;

  if (ozoneStringLength(&http_response->body))
    buffer_size += ozoneStringLength(&http_response->body);

  OzoneSocketChunkT* socket_chunk = ozoneAllocatorReserveOne(allocator, OzoneSocketChunkT);
  socket_chunk->buffer = ozoneAllocatorReserveMany(allocator, char, buffer_size);
  socket_chunk->length = buffer_size;

  char* cursor = socket_chunk->buffer;

  memcpy(cursor, ozoneStringBuffer(&version), ozoneStringLength(&version));
  cursor += ozoneStringLength(&version);
  *(cursor++) = ' ';

  memcpy(cursor, ozoneStringBuffer(&status), ozoneStringLength(&status));
  cursor += ozoneStringLength(&status);
  *(cursor++) = '\r';
  *(cursor++) = '\n';

  for (size_t header_index = 0; header_index < http_response->headers.length; header_index++) {
    OzoneHTTPHeaderT* header = &http_response->headers.elements[header_index];
    memcpy(cursor, ozoneStringBuffer(&header->name), ozoneStringLength(&header->name));
    cursor += ozoneStringLength(&header->name);
    *(cursor++) = ':';
    *(cursor++) = ' ';
    memcpy(cursor, ozoneStringBuffer(&header->value), ozoneStringLength(&header->value));
    cursor += ozoneStringLength(&header->value);
    *(cursor++) = '\r';
    *(cursor++) = '\n';
  }

  cursor[0] = '\r';
  cursor[1] = '\n';
  cursor += 2;

  if (ozoneStringLength(&http_response->body)) {
    memcpy(cursor, ozoneStringBuffer(&http_response->body), ozoneStringLength(&http_response->body));
  }

  return socket_chunk;
}

int ozoneHTTPBeginPipeline(OzoneHTTPContextT* context) {
  context->parsed_request = ozoneHTTPParseSocketChunks(context->allocator, context->raw_request);
  context->parsed_response = ozoneAllocatorReserveOne(context->allocator, OzoneHTTPResponseT);
  *context->parsed_response = (OzoneHTTPResponseT) { 0 };

  ozoneLogInfo("%s",
      ozoneStringScanBuffer(context->allocator, context->raw_request->buffer, context->raw_request->length,
          &ozoneString("\r"), OZONE_STRING_ENCODING_ISO_8859_1)
          .vector.elements);

  return 0;
}

int ozoneHTTPEndPipeline(OzoneHTTPContextT* context) {
  if (!context->parsed_response)
    return 0;

  OzoneHTTPResponseT* response = context->parsed_response;

  if (!response->code)
    response->code = ozoneStringLength(&response->body) ? 200 : 204;

  if (ozoneStringLength(&response->body)) {
    if (!ozoneHTTPGetHeaderValue(&response->headers, ozoneString("Content-Type")))
      ozoneHTTPAppendHeader(
          context->allocator, &response->headers, &ozoneString("Content-Type"), &ozoneString("text/plain"));

    if (!ozoneHTTPGetHeaderValue(&response->headers, ozoneString("Content-Length"))) {
      // todo: extract to helper
      char content_length[32] = { 0 };
      snprintf(content_length, sizeof(content_length), "%ld", ozoneStringLength(&response->body));

      size_t length = 0;
      while (content_length[length++] && length < sizeof(content_length))
        ;
      // end todo

      ozoneHTTPAppendHeader(context->allocator, &response->headers, &ozoneString("Content-Length"),
          &((OzoneStringT) {
              .vector = ((OzoneVectorCharT) {
                  .elements = content_length,
                  .length = length,
                  .capacity = length,
                  .capacity_increment = length,
              }),
              .encoding = OZONE_STRING_ENCODING_ISO_8859_1,
          }));
    }
  }

  context->raw_response = ozoneHTTPCreateSocketChunks(context->allocator, response);
  ozoneLogInfo("%s",
      ozoneStringScanBuffer(context->allocator, context->raw_response->buffer, context->raw_response->length,
          &ozoneString("\r"), OZONE_STRING_ENCODING_ISO_8859_1)
          .vector.elements);

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
