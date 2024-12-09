#ifndef OZONE_HTTP
#define OZONE_HTTP
#include "ozone_allocator.h"
#include "ozone_array.h"
#include "ozone_socket.h"

typedef unsigned short int OzoneHTTPStatusCodeT;

typedef struct OzoneHTTPHeaders
{
  OzoneArrayStringT *keys;
  OzoneArrayStringT *values;
  size_t count;
} OzoneHTTPHeadersT;

typedef struct OzoneHTTPRequest
{
  OzoneArrayStringT method;
  OzoneArrayStringT target;
  OzoneArrayStringT version;
  OzoneHTTPHeadersT headers;
  OzoneArrayStringT body;
} OzoneHTTPRequestT;

typedef struct OzoneHTTPResponse
{
  OzoneArrayStringT version;
  OzoneHTTPStatusCodeT code;
  OzoneHTTPHeadersT headers;
  OzoneArrayStringT body;
} OzoneHTTPResponseT;

OzoneArrayStringT *ozoneHTTPHeadersGetValue(OzoneHTTPHeadersT headers, OzoneArrayStringT key);

OzoneHTTPHeadersT *ozoneHTTPHeadersSetHeaders(OzoneAllocatorT *alloc, OzoneHTTPHeadersT *headers, OzoneArrayStringT *keys, OzoneArrayStringT *values, size_t set_count);

OzoneHTTPRequestT *ozoneHTTPRequestCreateFromString(OzoneAllocatorT *alloc, OzoneArrayStringT *input);

OzoneHTTPResponseT *ozoneHTTPResponseCreate(OzoneAllocatorT *alloc);

OzoneArrayStringT *ozoneHTTPResponseGetString(OzoneAllocatorT *alloc, OzoneHTTPResponseT *response);

OzoneArrayStringT ozoneHTTPStatusString(OzoneHTTPStatusCodeT status);

typedef struct OzoneHTTPHandlerParameter
{
  OzoneHTTPRequestT *request;
  OzoneHTTPResponseT *response;
} OzoneHTTPHandlerParameterT;

int ozoneHTTPSocketHandlerBegin(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param);
int ozoneHTTPSocketHandlerEnd(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param);
void ozoneHTTPSocketErrorHandler(OzoneAllocatorT *alloc, OzoneSocketHandlerParameterT *param, int error);

typedef int(OzoneHTTPHandlerT)(OzoneAllocatorT *alloc, OzoneHTTPHandlerParameterT *param);
int ozoneHTTPServe(unsigned short int port, OzoneHTTPHandlerT *handler);

#endif
