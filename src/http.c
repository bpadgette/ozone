#include "http.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

OZArrayStringT *ozHTTPHeadersGetValue(OZHTTPHeadersT headers, OZArrayStringT key)
{

  for (size_t key_index = 0; key_index < headers.count; key_index++)
  {
    if (ozArrayStringIs(&headers.keys[key_index], &key))
      return &headers.values[key_index];
  }

  return NULL;
}

OZHTTPHeadersT *ozHTTPHeadersSetHeaders(OZAllocatorT *alloc, OZHTTPHeadersT *init_headers, OZArrayStringT *keys, OZArrayStringT *values, size_t set_count)
{
  OZHTTPHeadersT *headers = init_headers ? init_headers : ozAllocatorReserveOne(alloc, OZHTTPHeadersT);

  int grow_count = set_count;
  int *push_indexes = ozAllocatorReserveMany(alloc, int, set_count);
  for (size_t set_count_index = 0; set_count_index < set_count; set_count_index++)
  {
    push_indexes[set_count_index] = 1;
    for (size_t old_key = 0; old_key < headers->count; old_key++)
    {
      if (ozArrayStringIs(&headers->keys[old_key], &keys[set_count_index]))
      {
        headers->values[old_key] = values[set_count_index];
        push_indexes[set_count_index]--;
        grow_count--;
        break;
      }
    }
  }

  if (grow_count == 0)
    return headers;

  size_t new_count = headers->count + grow_count;
  OZArrayStringT *new_keys = ozAllocatorReserveMany(alloc, OZArrayStringT, new_count);
  OZArrayStringT *new_values = ozAllocatorReserveMany(alloc, OZArrayStringT, new_count);
  size_t new_count_index = 0;
  for (; new_count_index < headers->count; new_count_index++)
  {
    new_keys[new_count_index] = headers->keys[new_count_index];
    new_values[new_count_index] = headers->values[new_count_index];
  }

  for (size_t set_count_index = 0; set_count_index < set_count; set_count_index++)
  {
    if (push_indexes[set_count_index])
    {
      new_keys[new_count_index] = keys[set_count_index];
      new_values[new_count_index] = values[set_count_index];
    }

    new_count_index++;
  }

  headers->keys = new_keys;
  headers->values = new_values;
  headers->count = new_count;

  return headers;
}

OZHTTPRequestT *ozHTTPRequestCreateFromString(OZAllocatorT *alloc, OZArrayStringT *input)
{
  OZArrayStringT *copy = ozArrayStringCopy(alloc, input);
  char *cursor = copy->data;

  OZHTTPRequestT *request = ozAllocatorReserveOne(alloc, OZHTTPRequestT);
  request->method = *ozArrayStringCreate(alloc, sizeof("CONNECT"));
  request->target = *ozArrayStringCreate(alloc, copy->length);
  request->version = *ozArrayStringCreate(alloc, sizeof("HTTP/1.1"));
  request->headers = (OZHTTPHeadersT){0};
  request->body = (OZArrayStringT){0};

  for (size_t method_index = 0; method_index < request->method.length; method_index++)
  {
    if (cursor[method_index] == ' ')
    {
      request->method.length = ++method_index;
      cursor += method_index;
      break;
    }

    request->method.data[method_index] = cursor[method_index];
  }

  for (size_t target_index = 0; target_index < request->target.length; target_index++)
  {
    if (cursor[target_index] == ' ')
    {
      request->target.length = ++target_index;
      cursor += target_index;
      break;
    }

    request->target.data[target_index] = cursor[target_index];
  }

  for (size_t version_index = 0; version_index < request->version.length; version_index++)
  {
    if (cursor[version_index] == '\r')
    {
      request->version.length = ++version_index;
      cursor += (version_index + 1);
      break;
    }

    request->version.data[version_index] = cursor[version_index];
  }

  char *header_key_start = NULL;
  char *header_key_end = NULL;
  char *header_value_start = NULL;
  for (; cursor < (copy->data + copy->length); cursor++)
  {
    if (!header_key_start && cursor[0] == '\r')
    {
      cursor += 2;
      break;
    }

    if (!header_key_start)
    {
      header_key_start = cursor;
      continue;
    }

    if (!header_key_end && cursor[0] == ':')
    {
      cursor[0] = '\0';
      header_key_end = cursor;
      cursor += 2;
      header_value_start = cursor;
      continue;
    }

    if (cursor[0] == '\r')
    {
      cursor[0] = '\0';
      ozHTTPHeadersSetHeaders(
          alloc,
          &request->headers,
          &(OZArrayStringT){
              .data = header_key_start,
              .length = header_key_end - header_key_start},
          &(OZArrayStringT){
              .data = header_value_start,
              .length = cursor - header_value_start},
          1);

      header_key_start = NULL;
      header_key_end = NULL;
      header_value_start = NULL;
      cursor++;
      continue;
    }
  }

  OZArrayStringT *content_length = ozHTTPHeadersGetValue(request->headers, ozArrayStringFromChars("Content-Length"));
  if (!content_length)
    return request;

  long content_length_value = strtol(content_length->data, NULL, 10);
  cursor[content_length_value] = '\0';
  request->body = (OZArrayStringT){.data = cursor, .length = content_length_value + 1};

  return request;
}

OZHTTPResponseT *ozHTTPResponseCreate(OZAllocatorT *alloc)
{
  OZHTTPResponseT *response = ozAllocatorReserveOne(alloc, OZHTTPResponseT);
  response->version = (OZArrayStringT){.data = "HTTP/1.1", .length = 9};
  response->code = 200;
  response->body = *ozArrayStringCreate(alloc, 1);
  return response;
}

OZArrayStringT *ozHTTPResponseGetString(OZAllocatorT *alloc, OZHTTPResponseT *response)
{
  OZArrayStringT status = ozHTTPStatusString(response->code);
  size_t content_length = fmax(response->body.length - 1, 0);
  char content_length_string[(int)((ceil(log10(content_length)) + 1) * sizeof(char))];
  sprintf(content_length_string, "%ld", content_length);
  ozHTTPHeadersSetHeaders(
      alloc,
      &response->headers,
      &ozArrayStringFromChars("Content-Length"),
      &ozArrayStringFromChars(content_length_string),
      1);

  size_t length = response->version.length + status.length + 1;
  for (size_t header_index = 0; header_index < response->headers.count; header_index++)
  {
    length += response->headers.keys[header_index].length + response->headers.values[header_index].length + 2;
  }
  length += content_length + 5;

  OZArrayStringT *output = ozArrayStringCreate(alloc, length);
  if (!output)
  {
    ozLogError("Could not allocate length of %ld for HTTP response", length);
    return NULL;
  }

  char *cursor = output->data;
  sprintf(cursor, "%s %s\r\n", response->version.data, status.data);
  cursor += response->version.length + status.length + 1;

  for (size_t header_index = 0; header_index < response->headers.count; header_index++)
  {
    sprintf(cursor,
            "%s: %s\r\n",
            response->headers.keys[header_index].data,
            response->headers.values[header_index].data);

    cursor += response->headers.keys[header_index].length + response->headers.values[header_index].length + 2;
  }

  snprintf(cursor, content_length + 5, "\r\n%s\r\n", response->body.data);

  return output;
}

OZArrayStringT ozHTTPStatusString(OZHTTPStatusCodeT status)
{
  switch (status)
  {
  case 100:
    return ozArrayStringFromChars("100 Continue");
  case 101:
    return ozArrayStringFromChars("101 Switching Protocols");
  case 102:
    return ozArrayStringFromChars("102 Processing");
  case 103:
    return ozArrayStringFromChars("103 Early Hints");
  case 200:
    return ozArrayStringFromChars("200 OK");
  case 201:
    return ozArrayStringFromChars("201 Created");
  case 202:
    return ozArrayStringFromChars("202 Accepted");
  case 203:
    return ozArrayStringFromChars("203 Non-Authoritative Information");
  case 204:
    return ozArrayStringFromChars("204 No Content");
  case 205:
    return ozArrayStringFromChars("205 Reset Content");
  case 206:
    return ozArrayStringFromChars("206 Partial Content");
  case 207:
    return ozArrayStringFromChars("207 Multi-Status");
  case 208:
    return ozArrayStringFromChars("208 Already Reported");
  case 226:
    return ozArrayStringFromChars("226 IM Used");
  case 300:
    return ozArrayStringFromChars("300 Multiple Choices");
  case 301:
    return ozArrayStringFromChars("301 Moved Permanently");
  case 302:
    return ozArrayStringFromChars("302 Found");
  case 303:
    return ozArrayStringFromChars("303 See Other");
  case 304:
    return ozArrayStringFromChars("304 Not Modified");
  case 305:
    return ozArrayStringFromChars("305 Use Proxy");
  case 307:
    return ozArrayStringFromChars("307 Temporary Redirect");
  case 308:
    return ozArrayStringFromChars("308 Permanent Redirect");
  case 400:
    return ozArrayStringFromChars("400 Bad Request");
  case 401:
    return ozArrayStringFromChars("401 Unauthorized");
  case 402:
    return ozArrayStringFromChars("402 Payment Required");
  case 403:
    return ozArrayStringFromChars("403 Forbidden");
  case 404:
    return ozArrayStringFromChars("404 Not Found");
  case 405:
    return ozArrayStringFromChars("405 Method Not Allowed");
  case 406:
    return ozArrayStringFromChars("406 Not Acceptable");
  case 407:
    return ozArrayStringFromChars("407 Proxy Authentication Required");
  case 408:
    return ozArrayStringFromChars("408 Request Timeout");
  case 409:
    return ozArrayStringFromChars("409 Conflict");
  case 410:
    return ozArrayStringFromChars("410 Gone");
  case 411:
    return ozArrayStringFromChars("411 Length Required");
  case 412:
    return ozArrayStringFromChars("412 Precondition Failed");
  case 413:
    return ozArrayStringFromChars("413 Content Too Large");
  case 414:
    return ozArrayStringFromChars("414 URI Too Long");
  case 415:
    return ozArrayStringFromChars("415 Unsupported Media Type");
  case 416:
    return ozArrayStringFromChars("416 Range Not Satisfiable");
  case 417:
    return ozArrayStringFromChars("417 Expectation Failed");
  case 418:
    return ozArrayStringFromChars("418 I'm a teapot");
  case 421:
    return ozArrayStringFromChars("421 Misdirected Request");
  case 422:
    return ozArrayStringFromChars("422 Unprocessable Content");
  case 423:
    return ozArrayStringFromChars("423 Locked");
  case 424:
    return ozArrayStringFromChars("424 Failed Dependency");
  case 425:
    return ozArrayStringFromChars("425 Too Early");
  case 426:
    return ozArrayStringFromChars("426 Upgrade Required");
  case 428:
    return ozArrayStringFromChars("428 Precondition Required");
  case 429:
    return ozArrayStringFromChars("429 Too Many Requests");
  case 431:
    return ozArrayStringFromChars("431 Request Header Fields Too Large");
  case 500:
    return ozArrayStringFromChars("500 Internal Server Error");
  case 501:
    return ozArrayStringFromChars("501 Not Implemented");
  case 502:
    return ozArrayStringFromChars("502 Bad Gateway");
  case 503:
    return ozArrayStringFromChars("503 Service Unavailable");
  case 504:
    return ozArrayStringFromChars("504 Gateway Timeout");
  case 505:
    return ozArrayStringFromChars("505 HTTP Version Not Supported");
  case 506:
    return ozArrayStringFromChars("506 Variant Also Negotiates");
  case 507:
    return ozArrayStringFromChars("507 Insufficient Storage");
  case 508:
    return ozArrayStringFromChars("508 Loop Detected");
  case 510:
    return ozArrayStringFromChars("510 Not Extended");
  case 511:
    return ozArrayStringFromChars("511 Network Authentication Required");
  default:
    ozLogError("Unknown HTTP status %d", status);
    return ozArrayStringFromChars("500 Internal Server Error");
  }
}

int ozHTTPSocketHandlerBegin(OZAllocatorT *alloc, OZSocketHandlerParameterT *param)
{
  ozLogDebug("Incoming request");

  OZHTTPRequestT *http_request = ozHTTPRequestCreateFromString(alloc, (OZSocketRequestT *)param->request);
  if (!http_request)
  {
    ozLogError("Could not create HTTP request from socket request, dropping request");
    return 1;
  }

  OZHTTPResponseT *http_response = ozHTTPResponseCreate(alloc);
  if (!http_response)
  {
    ozLogError("Could not create HTTP response, dropping request");
    return 1;
  }

  ozLogInfo("%s %s %s", http_request->version.data, http_request->method.data, http_request->target.data);
  param->request = http_request;
  param->response = http_response;
  return 0;
}

int ozHTTPSocketHandlerEnd(OZAllocatorT *alloc, OZSocketHandlerParameterT *param)
{
  ozLogInfo("%s %s",
            ((OZHTTPResponseT *)param->response)->version.data,
            ozHTTPStatusString(((OZHTTPResponseT *)param->response)->code).data);

  param->response = ozHTTPResponseGetString(alloc, (OZHTTPResponseT *)param->response);

  return 0;
}

void ozHTTPSocketErrorHandler(OZAllocatorT *alloc, OZSocketHandlerParameterT *param, int error)
{
  OZHTTPResponseT *response = (OZHTTPResponseT *)param->response;

  response->code = error >= 400 && error <= 511 ? error : 500;
  response->body = ozHTTPStatusString(response->code);
  ozLogError("%s %s", response->version.data, response->body.data);

  param->response = ozHTTPResponseGetString(alloc, (OZHTTPResponseT *)param->response);
}

int ozHTTPServe(unsigned short int port, OZHTTPHandlerT *handler)
{
  OZSocketHandlerT *handler_pipeline[] = {
      ozHTTPSocketHandlerBegin,
      (OZSocketHandlerT *)handler,
      ozHTTPSocketHandlerEnd};

  ozLogInfo("Serving at http://localhost:%d", port);
  return ozSocketServeTCP(
      (OZSocketConfigT){
          .error_handler = ozHTTPSocketErrorHandler,
          .handler_pipeline = handler_pipeline,
          .handler_pipeline_length = sizeof(handler_pipeline) / sizeof(OZSocketHandlerT *),
          .port = port});
}
