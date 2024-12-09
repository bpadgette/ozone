#include "ozone_http.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ozone_log.h"

OzoneArrayStringT *ozoneHTTPHeadersGetValue(OzoneHTTPHeadersT headers, OzoneArrayStringT key)
{

  for (size_t key_index = 0; key_index < headers.count; key_index++)
  {
    if (ozoneArrayStringIs(&headers.keys[key_index], &key))
      return &headers.values[key_index];
  }

  return NULL;
}

OzoneHTTPHeadersT *ozoneHTTPHeadersSetHeaders(OzoneAllocatorT *alloc, OzoneHTTPHeadersT *init_headers, OzoneArrayStringT *keys, OzoneArrayStringT *values, size_t set_count)
{
  OzoneHTTPHeadersT *headers = init_headers ? init_headers : ozoneAllocatorReserveOne(alloc, OzoneHTTPHeadersT);

  int grow_count = set_count;
  int *push_indexes = ozoneAllocatorReserveMany(alloc, int, set_count);
  for (size_t set_count_index = 0; set_count_index < set_count; set_count_index++)
  {
    push_indexes[set_count_index] = 1;
    for (size_t old_key = 0; old_key < headers->count; old_key++)
    {
      if (ozoneArrayStringIs(&headers->keys[old_key], &keys[set_count_index]))
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
  OzoneArrayStringT *new_keys = ozoneAllocatorReserveMany(alloc, OzoneArrayStringT, new_count);
  OzoneArrayStringT *new_values = ozoneAllocatorReserveMany(alloc, OzoneArrayStringT, new_count);
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

OzoneHTTPRequestT *ozoneHTTPRequestCreateFromString(OzoneAllocatorT *alloc, OzoneArrayStringT *input)
{
  OzoneArrayStringT *copy = ozoneArrayStringCopy(alloc, input);
  char *cursor = copy->data;

  OzoneHTTPRequestT *request = ozoneAllocatorReserveOne(alloc, OzoneHTTPRequestT);
  request->method = *ozoneArrayStringCreate(alloc, sizeof("CONNECT"));
  request->target = *ozoneArrayStringCreate(alloc, copy->length);
  request->version = *ozoneArrayStringCreate(alloc, sizeof("HTTP/1.1"));
  request->headers = (OzoneHTTPHeadersT){0};
  request->body = (OzoneArrayStringT){0};

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
      ozoneHTTPHeadersSetHeaders(
          alloc,
          &request->headers,
          &(OzoneArrayStringT){
              .data = header_key_start,
              .length = header_key_end - header_key_start},
          &(OzoneArrayStringT){
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

  OzoneArrayStringT *content_length = ozoneHTTPHeadersGetValue(request->headers, ozoneArrayStringFromChars("Content-Length"));
  if (!content_length)
    return request;

  long content_length_value = strtol(content_length->data, NULL, 10);
  cursor[content_length_value] = '\0';
  request->body = (OzoneArrayStringT){.data = cursor, .length = content_length_value + 1};

  return request;
}

OzoneHTTPResponseT *ozoneHTTPResponseCreate(OzoneAllocatorT *alloc)
{
  OzoneHTTPResponseT *response = ozoneAllocatorReserveOne(alloc, OzoneHTTPResponseT);
  response->version = (OzoneArrayStringT){.data = "HTTP/1.1", .length = 9};
  response->code = 200;
  response->body = *ozoneArrayStringCreate(alloc, 1);
  return response;
}

OzoneArrayStringT *ozoneHTTPResponseGetString(OzoneAllocatorT *alloc, OzoneHTTPResponseT *response)
{
  OzoneArrayStringT status = ozoneHTTPStatusString(response->code);
  size_t content_length = fmax(response->body.length - 1, 0);
  char content_length_string[(int)((ceil(log10(content_length)) + 1) * sizeof(char))];
  sprintf(content_length_string, "%ld", content_length);
  ozoneHTTPHeadersSetHeaders(
      alloc,
      &response->headers,
      &ozoneArrayStringFromChars("Content-Length"),
      &ozoneArrayStringFromChars(content_length_string),
      1);

  size_t length = response->version.length + status.length + 1;
  for (size_t header_index = 0; header_index < response->headers.count; header_index++)
  {
    length += response->headers.keys[header_index].length + response->headers.values[header_index].length + 2;
  }
  length += content_length + 5;

  OzoneArrayStringT *output = ozoneArrayStringCreate(alloc, length);
  if (!output)
  {
    ozoneLogError("Could not allocate length of %ld for HTTP response", length);
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

OzoneArrayStringT ozoneHTTPStatusString(OzoneHTTPStatusCodeT status)
{
  switch (status)
  {
  case 100:
    return ozoneArrayStringFromChars("100 Continue");
  case 101:
    return ozoneArrayStringFromChars("101 Switching Protocols");
  case 102:
    return ozoneArrayStringFromChars("102 Processing");
  case 103:
    return ozoneArrayStringFromChars("103 Early Hints");
  case 200:
    return ozoneArrayStringFromChars("200 OK");
  case 201:
    return ozoneArrayStringFromChars("201 Created");
  case 202:
    return ozoneArrayStringFromChars("202 Accepted");
  case 203:
    return ozoneArrayStringFromChars("203 Non-Authoritative Information");
  case 204:
    return ozoneArrayStringFromChars("204 No Content");
  case 205:
    return ozoneArrayStringFromChars("205 Reset Content");
  case 206:
    return ozoneArrayStringFromChars("206 Partial Content");
  case 207:
    return ozoneArrayStringFromChars("207 Multi-Status");
  case 208:
    return ozoneArrayStringFromChars("208 Already Reported");
  case 226:
    return ozoneArrayStringFromChars("226 IM Used");
  case 300:
    return ozoneArrayStringFromChars("300 Multiple Choices");
  case 301:
    return ozoneArrayStringFromChars("301 Moved Permanently");
  case 302:
    return ozoneArrayStringFromChars("302 Found");
  case 303:
    return ozoneArrayStringFromChars("303 See Other");
  case 304:
    return ozoneArrayStringFromChars("304 Not Modified");
  case 305:
    return ozoneArrayStringFromChars("305 Use Proxy");
  case 307:
    return ozoneArrayStringFromChars("307 Temporary Redirect");
  case 308:
    return ozoneArrayStringFromChars("308 Permanent Redirect");
  case 400:
    return ozoneArrayStringFromChars("400 Bad Request");
  case 401:
    return ozoneArrayStringFromChars("401 Unauthorized");
  case 402:
    return ozoneArrayStringFromChars("402 Payment Required");
  case 403:
    return ozoneArrayStringFromChars("403 Forbidden");
  case 404:
    return ozoneArrayStringFromChars("404 Not Found");
  case 405:
    return ozoneArrayStringFromChars("405 Method Not Allowed");
  case 406:
    return ozoneArrayStringFromChars("406 Not Acceptable");
  case 407:
    return ozoneArrayStringFromChars("407 Proxy Authentication Required");
  case 408:
    return ozoneArrayStringFromChars("408 Request Timeout");
  case 409:
    return ozoneArrayStringFromChars("409 Conflict");
  case 410:
    return ozoneArrayStringFromChars("410 Gone");
  case 411:
    return ozoneArrayStringFromChars("411 Length Required");
  case 412:
    return ozoneArrayStringFromChars("412 Precondition Failed");
  case 413:
    return ozoneArrayStringFromChars("413 Content Too Large");
  case 414:
    return ozoneArrayStringFromChars("414 URI Too Long");
  case 415:
    return ozoneArrayStringFromChars("415 Unsupported Media Type");
  case 416:
    return ozoneArrayStringFromChars("416 Range Not Satisfiable");
  case 417:
    return ozoneArrayStringFromChars("417 Expectation Failed");
  case 418:
    return ozoneArrayStringFromChars("418 I'm a teapot");
  case 421:
    return ozoneArrayStringFromChars("421 Misdirected Request");
  case 422:
    return ozoneArrayStringFromChars("422 Unprocessable Content");
  case 423:
    return ozoneArrayStringFromChars("423 Locked");
  case 424:
    return ozoneArrayStringFromChars("424 Failed Dependency");
  case 425:
    return ozoneArrayStringFromChars("425 Too Early");
  case 426:
    return ozoneArrayStringFromChars("426 Upgrade Required");
  case 428:
    return ozoneArrayStringFromChars("428 Precondition Required");
  case 429:
    return ozoneArrayStringFromChars("429 Too Many Requests");
  case 431:
    return ozoneArrayStringFromChars("431 Request Header Fields Too Large");
  case 500:
    return ozoneArrayStringFromChars("500 Internal Server Error");
  case 501:
    return ozoneArrayStringFromChars("501 Not Implemented");
  case 502:
    return ozoneArrayStringFromChars("502 Bad Gateway");
  case 503:
    return ozoneArrayStringFromChars("503 Service Unavailable");
  case 504:
    return ozoneArrayStringFromChars("504 Gateway Timeout");
  case 505:
    return ozoneArrayStringFromChars("505 HTTP Version Not Supported");
  case 506:
    return ozoneArrayStringFromChars("506 Variant Also Negotiates");
  case 507:
    return ozoneArrayStringFromChars("507 Insufficient Storage");
  case 508:
    return ozoneArrayStringFromChars("508 Loop Detected");
  case 510:
    return ozoneArrayStringFromChars("510 Not Extended");
  case 511:
    return ozoneArrayStringFromChars("511 Network Authentication Required");
  default:
    ozoneLogError("Unknown HTTP status %d", status);
    return ozoneArrayStringFromChars("500 Internal Server Error");
  }
}

int ozoneHTTPSocketHandlerBegin(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param)
{
  ozoneLogDebug("Incoming request");

  OzoneHTTPRequestT *http_request = ozoneHTTPRequestCreateFromString(alloc, (OzoneSocketRequestT *)param->request);
  if (!http_request)
  {
    ozoneLogError("Could not create HTTP request from socket request, dropping request");
    return 1;
  }

  OzoneHTTPResponseT *http_response = ozoneHTTPResponseCreate(alloc);
  if (!http_response)
  {
    ozoneLogError("Could not create HTTP response, dropping request");
    return 1;
  }

  ozoneLogInfo("%s %s %s", http_request->version.data, http_request->method.data, http_request->target.data);
  param->request = http_request;
  param->response = http_response;
  return 0;
}

int ozoneHTTPSocketHandlerEnd(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param)
{
  ozoneLogInfo("%s %s",
               ((OzoneHTTPResponseT *)param->response)->version.data,
               ozoneHTTPStatusString(((OzoneHTTPResponseT *)param->response)->code).data);

  param->response = ozoneHTTPResponseGetString(alloc, (OzoneHTTPResponseT *)param->response);

  return 0;
}

void ozoneHTTPSocketErrorHandler(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param, int error)
{
  OzoneHTTPResponseT *response = (OzoneHTTPResponseT *)param->response;

  response->code = error >= 400 && error <= 511 ? error : 500;
  response->body = ozoneHTTPStatusString(response->code);
  ozoneLogError("%s %s", response->version.data, response->body.data);

  param->response = ozoneHTTPResponseGetString(alloc, (OzoneHTTPResponseT *)param->response);
}

int ozoneHTTPServe(unsigned short int port, OzoneHTTPHandlerT *handler)
{
  OzoneSocketHandlerT *handler_pipeline[] = {
      ozoneHTTPSocketHandlerBegin,
      (OzoneSocketHandlerT *)handler,
      ozoneHTTPSocketHandlerEnd};

  ozoneLogInfo("Serving at http://localhost:%d", port);
  return ozoneSocketServeTCP(
      (OzoneSocketConfigT){
          .error_handler = ozoneHTTPSocketErrorHandler,
          .handler_pipeline = handler_pipeline,
          .handler_pipeline_length = sizeof(handler_pipeline) / sizeof(OzoneSocketHandlerT *),
          .port = port});
}
