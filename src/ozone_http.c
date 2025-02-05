#include "ozone_http.h"

#include "ozone_log.h"
#include <stdlib.h>
#include <string.h>

OzoneHTTPVersion ozoneHTTPParseVersion(const OzoneString* version_string) {
  if (ozoneStringCompare(version_string, &ozoneStringConstant("HTTP/1.1")) == 0)
    return OZONE_HTTP_VERSION_1_1;
  if (ozoneStringCompare(version_string, &ozoneStringConstant("HTTP/1.0")) == 0)
    return OZONE_HTTP_VERSION_1_0;

  ozoneLogWarn("Could not parse HTTP version");
  return OZONE_HTTP_VERSION_UNKNOWN;
}

OzoneHTTPMethod ozoneHTTPParseMethod(const OzoneString* method_string) {
  if (ozoneStringCompare(method_string, &ozoneStringConstant("GET")) == 0)
    return OZONE_HTTP_METHOD_GET;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("HEAD")) == 0)
    return OZONE_HTTP_METHOD_HEAD;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("POST")) == 0)
    return OZONE_HTTP_METHOD_POST;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("PUT")) == 0)
    return OZONE_HTTP_METHOD_PUT;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("DELETE")) == 0)
    return OZONE_HTTP_METHOD_DELETE;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("CONNECT")) == 0)
    return OZONE_HTTP_METHOD_CONNECT;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("OPTIONS")) == 0)
    return OZONE_HTTP_METHOD_OPTIONS;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("PATCH")) == 0)
    return OZONE_HTTP_METHOD_PATCH;
  if (ozoneStringCompare(method_string, &ozoneStringConstant("TRACE")) == 0)
    return OZONE_HTTP_METHOD_TRACE;

  ozoneLogWarn("Could not parse HTTP method");
  return OZONE_HTTP_METHOD_UNKNOWN;
}

OzoneString* ozoneHTTPStatusText(OzoneAllocator* allocator, OzoneHTTPStatusCode status) {
  switch (status) {
  case 100:
    return ozoneString(allocator, "100 Continue");
  case 101:
    return ozoneString(allocator, "101 Switching Protocols");
  case 102:
    return ozoneString(allocator, "102 Processing");
  case 103:
    return ozoneString(allocator, "103 Early Hints");
  case 200:
    return ozoneString(allocator, "200 OK");
  case 201:
    return ozoneString(allocator, "201 Created");
  case 202:
    return ozoneString(allocator, "202 Accepted");
  case 203:
    return ozoneString(allocator, "203 Non-Authoritative Information");
  case 204:
    return ozoneString(allocator, "204 No Content");
  case 205:
    return ozoneString(allocator, "205 Reset Content");
  case 206:
    return ozoneString(allocator, "206 Partial Content");
  case 207:
    return ozoneString(allocator, "207 Multi-Status");
  case 208:
    return ozoneString(allocator, "208 Already Reported");
  case 226:
    return ozoneString(allocator, "226 IM Used");
  case 300:
    return ozoneString(allocator, "300 Multiple Choices");
  case 301:
    return ozoneString(allocator, "301 Moved Permanently");
  case 302:
    return ozoneString(allocator, "302 Found");
  case 303:
    return ozoneString(allocator, "303 See Other");
  case 304:
    return ozoneString(allocator, "304 Not Modified");
  case 305:
    return ozoneString(allocator, "305 Use Proxy");
  case 307:
    return ozoneString(allocator, "307 Temporary Redirect");
  case 308:
    return ozoneString(allocator, "308 Permanent Redirect");
  case 400:
    return ozoneString(allocator, "400 Bad Request");
  case 401:
    return ozoneString(allocator, "401 Unauthorized");
  case 402:
    return ozoneString(allocator, "402 Payment Required");
  case 403:
    return ozoneString(allocator, "403 Forbidden");
  case 404:
    return ozoneString(allocator, "404 Not Found");
  case 405:
    return ozoneString(allocator, "405 Method Not Allowed");
  case 406:
    return ozoneString(allocator, "406 Not Acceptable");
  case 407:
    return ozoneString(allocator, "407 Proxy Authentication Required");
  case 408:
    return ozoneString(allocator, "408 Request Timeout");
  case 409:
    return ozoneString(allocator, "409 Conflict");
  case 410:
    return ozoneString(allocator, "410 Gone");
  case 411:
    return ozoneString(allocator, "411 Length Required");
  case 412:
    return ozoneString(allocator, "412 Precondition Failed");
  case 413:
    return ozoneString(allocator, "413 Content Too Large");
  case 414:
    return ozoneString(allocator, "414 URI Too Long");
  case 415:
    return ozoneString(allocator, "415 Unsupported Media Type");
  case 416:
    return ozoneString(allocator, "416 Range Not Satisfiable");
  case 417:
    return ozoneString(allocator, "417 Expectation Failed");
  case 418:
    return ozoneString(allocator, "418 I'm a teapot");
  case 421:
    return ozoneString(allocator, "421 Misdirected Request");
  case 422:
    return ozoneString(allocator, "422 Unprocessable Content");
  case 423:
    return ozoneString(allocator, "423 Locked");
  case 424:
    return ozoneString(allocator, "424 Failed Dependency");
  case 425:
    return ozoneString(allocator, "425 Too Early");
  case 426:
    return ozoneString(allocator, "426 Upgrade Required");
  case 428:
    return ozoneString(allocator, "428 Precondition Required");
  case 429:
    return ozoneString(allocator, "429 Too Many Requests");
  case 431:
    return ozoneString(allocator, "431 Request Header Fields Too Large");
  case 500:
    return ozoneString(allocator, "500 Internal Server Error");
  case 501:
    return ozoneString(allocator, "501 Not Implemented");
  case 502:
    return ozoneString(allocator, "502 Bad Gateway");
  case 503:
    return ozoneString(allocator, "503 Service Unavailable");
  case 504:
    return ozoneString(allocator, "504 Gateway Timeout");
  case 505:
    return ozoneString(allocator, "505 HTTP Version Not Supported");
  case 506:
    return ozoneString(allocator, "506 Variant Also Negotiates");
  case 507:
    return ozoneString(allocator, "507 Insufficient Storage");
  case 508:
    return ozoneString(allocator, "508 Loop Detected");
  case 510:
    return ozoneString(allocator, "510 Not Extended");
  case 511:
    return ozoneString(allocator, "511 Network Authentication Required");
  default:
    ozoneLogWarn("Unknown HTTP status %d", status);
    return ozoneString(allocator, "500 Internal Server Error");
  }
}

#define OZONE_HTTP_PARSING_METHOD 1
#define OZONE_HTTP_PARSING_TARGET 2
#define OZONE_HTTP_PARSING_VERSION 3
#define OZONE_HTTP_PARSING_FIRST_LINE_RETURN 4
#define OZONE_HTTP_PARSING_HEADER_NAME 5
#define OZONE_HTTP_PARSING_HEADER_WHITESPACE 6
#define OZONE_HTTP_PARSING_HEADER_VALUE 7
#define OZONE_HTTP_PARSING_HEADER_LINE_RETURN 8
#define OZONE_HTTP_PARSING_HEADER_FINAL_LINE_RETURN 9
#define OZONE_HTTP_PARSING_BODY 10

OzoneHTTPRequest* ozoneHTTPParseSocketRequest(OzoneAllocator* allocator, const OzoneStringVector* socket_request) {
  OzoneHTTPRequest* http_request = ozoneAllocatorReserveOne(allocator, OzoneHTTPRequest);

  OzoneString* token = ozoneString(allocator, "");
  OzoneString* header_name = ozoneString(allocator, "");
  size_t content_length = 0;

  int parsing = OZONE_HTTP_PARSING_METHOD;
  OzoneString* chunk;
  ozoneVectorForEach(chunk, socket_request) {
    for (size_t string_index = 0; string_index < ozoneStringLength(chunk); string_index++) {
      char cursor = ozoneStringBufferAt(chunk, string_index);
      switch (parsing) {
      case OZONE_HTTP_PARSING_METHOD: {
        if (cursor == ' ' || ozoneStringLength(token) > sizeof("CONNECT")) {
          http_request->method = ozoneHTTPParseMethod(token);
          if (!http_request->method) {
            return NULL;
          }

          ozoneStringClear(token);
          parsing = OZONE_HTTP_PARSING_TARGET;
        } else {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      case OZONE_HTTP_PARSING_TARGET: {
        if (cursor == ' ') {
          http_request->target = *ozoneStringCopy(allocator, token);
          ozoneStringClear(token);
          parsing = OZONE_HTTP_PARSING_VERSION;
        } else {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      case OZONE_HTTP_PARSING_VERSION: {
        if (cursor == '\r') {
          http_request->version = ozoneHTTPParseVersion(token);
          ozoneStringClear(token);
          parsing = OZONE_HTTP_PARSING_FIRST_LINE_RETURN;
        } else {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      case OZONE_HTTP_PARSING_FIRST_LINE_RETURN: {
        if (cursor == '\n') {
          parsing = OZONE_HTTP_PARSING_HEADER_NAME;
        }
        break;
      }
      case OZONE_HTTP_PARSING_HEADER_NAME: {
        if (cursor == '\r') {
          parsing = OZONE_HTTP_PARSING_HEADER_FINAL_LINE_RETURN;
        } else if (cursor == ':') {
          parsing = OZONE_HTTP_PARSING_HEADER_WHITESPACE;
        } else {
          ozoneStringAppend(allocator, header_name, cursor);
        }
        break;
      }
      case OZONE_HTTP_PARSING_HEADER_WHITESPACE: {
        if (cursor > ' ') {
          ozoneStringAppend(allocator, token, cursor);
          parsing = OZONE_HTTP_PARSING_HEADER_VALUE;
        }
        break;
      }
      case OZONE_HTTP_PARSING_HEADER_VALUE: {
        if (cursor == '\r') {
          ozoneStringMapInsert(allocator, &http_request->headers, header_name, token);
          ozoneStringClear(header_name);
          ozoneStringClear(token);
          parsing = OZONE_HTTP_PARSING_HEADER_LINE_RETURN;
        } else {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      case OZONE_HTTP_PARSING_HEADER_LINE_RETURN: {
        if (cursor == '\n') {
          parsing = OZONE_HTTP_PARSING_HEADER_NAME;
        }
        break;
      }
      case OZONE_HTTP_PARSING_HEADER_FINAL_LINE_RETURN: {
        if (cursor == '\n') {
          const OzoneString* content_length_header
              = ozoneStringMapFindValue(&http_request->headers, &ozoneStringConstant("Content-Length"));
          if (!content_length_header || ozoneStringToInteger(content_length_header) <= 0)
            return http_request;

          content_length = (size_t)ozoneStringToInteger(content_length_header);
          parsing = OZONE_HTTP_PARSING_BODY;
        }
        break;
      }
      case OZONE_HTTP_PARSING_BODY: {
        ozoneStringAppend(allocator, token, cursor);
        if (ozoneStringLength(token) == content_length) {
          http_request->body = *token;
          return http_request;
        }
        break;
      }
      }
    }
  }

  return http_request;
}

OzoneStringVector* ozoneHTTPRenderResponse(OzoneAllocator* allocator, OzoneHTTPResponse* http_response) {
  OzoneString* response;
  if (http_response->version == OZONE_HTTP_VERSION_1_0) {
    response = ozoneString(allocator, "HTTP/1.0 ");
  } else {
    response = ozoneString(allocator, "HTTP/1.1 ");
  }

  ozoneStringConcatenate(allocator, response, ozoneHTTPStatusText(allocator, http_response->code));
  ozoneStringAppend(allocator, response, '\r');
  ozoneStringAppend(allocator, response, '\n');

  for (size_t header_index = 0; header_index < ozoneVectorLength(&http_response->headers.keys); header_index++) {
    ozoneStringConcatenate(allocator, response, &ozoneVectorAt(&http_response->headers.keys, header_index));
    ozoneStringAppend(allocator, response, ':');
    ozoneStringAppend(allocator, response, ' ');
    ozoneStringConcatenate(allocator, response, &ozoneVectorAt(&http_response->headers.values, header_index));
    ozoneStringAppend(allocator, response, '\r');
    ozoneStringAppend(allocator, response, '\n');
  }

  ozoneStringAppend(allocator, response, '\r');
  ozoneStringAppend(allocator, response, '\n');

  if (ozoneStringLength(&http_response->body))
    ozoneStringConcatenate(allocator, response, &http_response->body);

  // Keep the chunked response format simply because it could be useful for HTTP/2 or multi-part bodies etc.
  OzoneStringVector* chunks = ozoneAllocatorReserveOne(allocator, OzoneStringVector);
  ozoneVectorPushOzoneString(allocator, chunks, response);
  return chunks;
}

int ozoneHTTPBeginPipeline(OzoneHTTPEvent* event) {
  OzoneString* first_line = ozoneString(event->allocator, "");
  OzoneString* chunk;
  ozoneVectorForEach(chunk, &event->raw_socket_request) {
    for (size_t string_index = 0; string_index < ozoneStringLength(chunk); string_index++) {
      char cursor = ozoneStringBufferAt(chunk, string_index);
      if (cursor == '\r') {
        ozoneLogInfo("%s", ozoneStringBuffer(first_line));
        chunk = ozoneVectorEnd(&event->raw_socket_request);
        break;
      }

      ozoneStringAppend(event->allocator, first_line, cursor);
    }
  }

  event->request = ozoneHTTPParseSocketRequest(event->allocator, &event->raw_socket_request);
  event->response = ozoneAllocatorReserveOne(event->allocator, OzoneHTTPResponse);

  return 0;
}

int ozoneHTTPEndPipeline(OzoneHTTPEvent* event) {
  if (!event->response)
    return 0;

  OzoneHTTPResponse* response = event->response;
  OzoneHTTPVersion http_version = event->request->version;
  if (http_version)
    response->version = http_version;

  if (response->version == OZONE_HTTP_VERSION_1_1)
    ozoneStringMapInsert(
        event->allocator, &response->headers, &ozoneStringConstant("Connection"), &ozoneStringConstant("keep-alive"));

  if (!response->code)
    response->code = ozoneStringLength(&response->body) ? 200 : 204;

  if (ozoneStringLength(&response->body)
      && !ozoneStringMapFindValue(&response->headers, &ozoneStringConstant("Content-Type")))
    ozoneStringMapInsert(
        event->allocator, &response->headers, &ozoneStringConstant("Content-Type"), &ozoneStringConstant("text/plain"));

  ozoneStringMapInsert(
      event->allocator,
      &response->headers,
      &ozoneStringConstant("Content-Length"),
      ozoneStringFromInteger(event->allocator, ozoneStringLength(&response->body)));

  event->raw_socket_response = *ozoneHTTPRenderResponse(event->allocator, response);

  OzoneString* first_line = ozoneString(event->allocator, "");
  OzoneString* chunk;
  ozoneVectorForEach(chunk, &event->raw_socket_response) {
    for (size_t string_index = 0; string_index < ozoneStringLength(chunk); string_index++) {
      char cursor = ozoneStringBufferAt(chunk, string_index);
      if (cursor == '\r') {
        ozoneLogInfo("%s", ozoneStringBuffer(first_line));
        return 0;
      }

      ozoneStringAppend(event->allocator, first_line, cursor);
    }
  }

  ozoneLogError("HTTP response is malformed");

  return 0;
}

int ozoneHTTPServe(OzoneHTTPConfig* config) {
  OzoneAllocator* allocator = ozoneAllocatorCreate(1024);
  OzoneSocketHandlerRefVector http_pipeline = (OzoneSocketHandlerRefVector) { 0 };

  OzoneSocketHandlerRef begin = (OzoneSocketHandlerRef)ozoneHTTPBeginPipeline;
  OzoneSocketHandlerRef end = (OzoneSocketHandlerRef)ozoneHTTPEndPipeline;

  ozoneVectorPushOzoneSocketHandlerRef(allocator, &http_pipeline, &begin);

  OzoneSocketHandlerRef* handler;
  ozoneVectorForEach(handler, &config->handler_pipeline) {
    ozoneVectorPushOzoneSocketHandlerRef(allocator, &http_pipeline, handler);
  }

  ozoneVectorPushOzoneSocketHandlerRef(allocator, &http_pipeline, &end);

  ozoneLogInfo("Serving at http://localhost:%d", config->port);

  OzoneSocketConfig socket_config = (OzoneSocketConfig) {
    .handler_pipeline = http_pipeline,
    .port = config->port,
    .handler_context = config->handler_context,
  };

  int return_code = ozoneSocketServeTCP(&socket_config);
  ozoneAllocatorDelete(allocator);
  return return_code;
}
