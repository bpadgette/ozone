#ifndef OZONE_HTTP
#define OZONE_HTTP
#include "ozone_allocator.h"
#include "ozone_array.h"
#include "ozone_socket.h"

typedef unsigned short int OzoneHTTPStatusCodeT;

typedef struct OzoneHTTPHeaders
{
  OzoneCharArrayT *keys;
  OzoneCharArrayT *values;
  size_t count;
} OzoneHTTPHeadersT;

typedef struct OzoneHTTPRequest
{
  OzoneCharArrayT method;
  OzoneCharArrayT target;
  OzoneCharArrayT version;
  OzoneHTTPHeadersT headers;
  OzoneCharArrayT body;
} OzoneHTTPRequestT;

typedef struct OzoneHTTPResponse
{
  OzoneCharArrayT version;
  OzoneHTTPStatusCodeT code;
  OzoneHTTPHeadersT headers;
  OzoneCharArrayT body;
} OzoneHTTPResponseT;

OzoneCharArrayT *ozoneHTTPHeadersGetValue(OzoneHTTPHeadersT headers, OzoneCharArrayT key);

OzoneHTTPHeadersT *ozoneHTTPHeadersSetHeaders(OzoneAllocatorT *alloc, OzoneHTTPHeadersT *headers, OzoneCharArrayT *keys, OzoneCharArrayT *values, size_t set_count);

OzoneHTTPRequestT *ozoneHTTPRequestCreateFromString(OzoneAllocatorT *alloc, OzoneCharArrayT *input);

OzoneHTTPResponseT *ozoneHTTPResponseCreate(OzoneAllocatorT *alloc);

OzoneCharArrayT *ozoneHTTPResponseGetString(OzoneAllocatorT *alloc, OzoneHTTPResponseT *response);

OzoneCharArrayT ozoneHTTPStatusString(OzoneHTTPStatusCodeT status);

typedef struct OzoneHTTPHandlerContext
{
  OzoneAllocatorT *allocator;
  OzoneHTTPRequestT *request;
  OzoneHTTPResponseT *response;
} OzoneHTTPHandlerContextT;

int ozoneHTTPSocketHandlerBegin(OzoneSocketHandlerContextT *context);
int ozoneHTTPSocketHandlerEnd(OzoneSocketHandlerContextT *context);
void ozoneHTTPSocketErrorHandler(OzoneSocketHandlerContextT *context, int error);

typedef int(OzoneHTTPHandlerT)(OzoneHTTPHandlerContextT *context);

int ozoneHTTPServe(unsigned short int port, OzoneHTTPHandlerT *handler);

#endif
