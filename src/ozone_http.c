#include "ozone_http.h"

#include "ozone_log.h"
#include <stdlib.h>
#include <string.h>

#define OZONE_REMAINING_CURSOR_SIZE (buffer + buffer_size - cursor)

OzoneHTTPVersion ozoneHTTPParseVersion(const OzoneString* version_string) {
  if (ozoneStringCompare(version_string, &ozoneString("HTTP/1.1")) == 0)
    return OZONE_HTTP_VERSION_1_1;
  if (ozoneStringCompare(version_string, &ozoneString("HTTP/1.0")) == 0)
    return OZONE_HTTP_VERSION_1_0;

  ozoneLogWarn("Could not parse HTTP version");
  return OZONE_HTTP_VERSION_UNKNOWN;
}

OzoneHTTPMethod ozoneHTTPParseMethod(const OzoneString* method_string) {
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

OzoneString ozoneHTTPStatusString(OzoneHTTPStatusCode status) {
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

OzoneHTTPRequest* ozoneHTTPParseSocketChunks(OzoneAllocator* allocator, const OzoneSocketChunk* socket_request) {
  size_t buffer_size = 0;

  // todo: extract to helper
  const OzoneSocketChunk* chunk = socket_request;
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

  OzoneHTTPRequest* http_request = ozoneAllocatorReserveOne(allocator, OzoneHTTPRequest);
  *http_request = (OzoneHTTPRequest) { 0 };

  OzoneString method_string = ozoneStringFromBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString(" "));
  if (OZONE_HTTP_METHOD_UNKNOWN == (http_request->method = ozoneHTTPParseMethod(&method_string)))
    return NULL;
  cursor += ozoneStringLength(&method_string) + 1;

  http_request->target = ozoneStringFromBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString(" "));
  cursor += ozoneStringLength(&http_request->target) + 1;

  OzoneString http_version_string
      = ozoneStringFromBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString("\r\n"));
  if (OZONE_HTTP_VERSION_UNKNOWN == (http_request->version = ozoneHTTPParseVersion(&http_version_string)))
    return NULL;
  cursor += ozoneStringLength(&http_version_string) + 2;

  while (buffer + buffer_size > (cursor + 1) && cursor[0] != '\r') {
    OzoneString key = ozoneStringFromBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString(":"));
    if (!ozoneStringLength(&key))
      break;

    cursor += ozoneStringLength(&key) + 2;

    OzoneString value = ozoneStringFromBuffer(allocator, cursor, OZONE_REMAINING_CURSOR_SIZE, &ozoneString("\r\n"));
    if (!ozoneStringLength(&value))
      break;

    ozoneStringPushKeyValue(allocator, &http_request->headers, &key, &value);
    cursor += ozoneStringLength(&value) + 2;
  }

  cursor += 2;

  OzoneString* content_length_string
      = ozoneStringKeyValueVectorFind(&http_request->headers, &ozoneString("Content-Length"));
  if (!content_length_string)
    return http_request;

  long int content_length = strtol(ozoneStringBuffer(content_length_string), NULL, 10);
  if (content_length == 0)
    return http_request;

  if (content_length > OZONE_REMAINING_CURSOR_SIZE) {
    ozoneLogWarn(
        "HTTP request specified a Content-Length of %ld but only %ld bytes remain in the buffer, dropping request body",
        content_length,
        OZONE_REMAINING_CURSOR_SIZE);
    return http_request;
  }

  cursor[content_length + 1] = '\0';
  http_request->body = ((OzoneString) {
      .vector = ((OzoneByteVector) {
          .elements = cursor,
          .length = content_length + 1,
          .capacity = content_length + 1,
      }),
  });

  return http_request;
}

OzoneSocketChunk* ozoneHTTPCreateSocketChunks(OzoneAllocator* allocator, OzoneHTTPResponse* http_response) {
  // todo: HTTP versioned response handling; upgrading and downgrading
  OzoneString version = ozoneString("HTTP/1.0");
  OzoneString status = ozoneHTTPStatusString(http_response->code);

  size_t buffer_size = ozoneStringLength(&version) + ozoneStringLength(&status) + 3;

  OzoneStringKeyValue* header;
  ozoneVectorForEach(header, &http_response->headers) {
    buffer_size += ozoneStringLength(&header->key) + 2 + ozoneStringLength(&header->value) + 2;
  }

  buffer_size += 2;

  if (ozoneStringLength(&http_response->body))
    buffer_size += ozoneStringLength(&http_response->body);

  OzoneSocketChunk* socket_chunk = ozoneAllocatorReserveOne(allocator, OzoneSocketChunk);
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

  ozoneVectorForEach(header, &http_response->headers) {
    memcpy(cursor, ozoneStringBuffer(&header->key), ozoneStringLength(&header->key));
    cursor += ozoneStringLength(&header->key);
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

int ozoneHTTPBeginPipeline(OzoneHTTPEvent* event, void* context) {
  (void)context;

  event->request = ozoneHTTPParseSocketChunks(event->allocator, event->raw_socket_request);
  event->response = ozoneAllocatorReserveOne(event->allocator, OzoneHTTPResponse);
  *event->response = (OzoneHTTPResponse) { 0 };

  ozoneLogInfo(
      "%s",
      ozoneStringFromBuffer(
          event->allocator, event->raw_socket_request->buffer, event->raw_socket_request->length, &ozoneString("\r"))
          .vector.elements);

  return 0;
}

int ozoneHTTPEndPipeline(OzoneHTTPEvent* event, void* context) {
  (void)context;

  if (!event->response)
    return 0;

  OzoneHTTPResponse* response = event->response;

  if (!response->code)
    response->code = ozoneStringLength(&response->body) ? 200 : 204;

  if (ozoneStringLength(&response->body)) {
    if (!ozoneStringKeyValueVectorFind(&response->headers, &ozoneString("Content-Type")))
      ozoneStringPushKeyValue(
          event->allocator, &response->headers, &ozoneString("Content-Type"), &ozoneString("text/plain"));

    if (!ozoneStringKeyValueVectorFind(&response->headers, &ozoneString("Content-Length"))) {
      // todo: extract to helper
      char content_length[32] = { 0 };
      snprintf(content_length, sizeof(content_length), "%ld", ozoneStringLength(&response->body));

      size_t length = 0;
      while (content_length[length++] && length < sizeof(content_length))
        ;
      // end todo

      ozoneStringPushKeyValue(
          event->allocator,
          &response->headers,
          &ozoneString("Content-Length"),
          &((OzoneString) {
              .vector = ((OzoneByteVector) {
                  .elements = content_length,
                  .length = length,
                  .capacity = length,
              }),
          }));
    }
  }

  event->raw_socket_response = ozoneHTTPCreateSocketChunks(event->allocator, response);
  ozoneLogInfo(
      "%s",
      ozoneStringFromBuffer(
          event->allocator, event->raw_socket_response->buffer, event->raw_socket_response->length, &ozoneString("\r"))
          .vector.elements);

  return 0;
}

int ozoneHTTPServe(OzoneAllocator* allocator, OzoneHTTPConfig config) {
  OzoneSocketHandlerRefVector* http_pipeline = ozoneAllocatorReserveOne(allocator, OzoneSocketHandlerRefVector);
  *http_pipeline = (OzoneSocketHandlerRefVector) { 0 };

  pushOzoneSocketHandlerRef(allocator, http_pipeline, (OzoneSocketHandlerRef)ozoneHTTPBeginPipeline);

  OzoneSocketHandlerRef* handler;
  ozoneVectorForEach(handler, &config.handler_pipeline) {
    pushOzoneSocketHandlerRef(allocator, http_pipeline, *handler);
  }

  pushOzoneSocketHandlerRef(allocator, http_pipeline, (OzoneSocketHandlerRef)ozoneHTTPEndPipeline);

  ozoneLogInfo("Serving at http://localhost:%d", config.port);

  OzoneSocketConfig socket_config = (OzoneSocketConfig) {
    .handler_pipeline = (*http_pipeline),
    .port = config.port,
    .handler_context = config.handler_context,
  };

  int return_code = ozoneSocketServeTCP(socket_config);
  ozoneAllocatorDelete(allocator);

  return return_code;
}
