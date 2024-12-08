#ifndef OZONE_HTTP
#define OZONE_HTTP
#include "allocator.h"
#include "array.h"
#include "socket.h"

typedef unsigned short int OZHTTPStatusCodeT;

typedef struct OZHTTPHeaders
{
  OZArrayStringT *keys;
  OZArrayStringT *values;
  size_t count;
} OZHTTPHeadersT;

typedef struct OZHTTPRequest
{
  OZArrayStringT method;
  OZArrayStringT target;
  OZArrayStringT version;
  OZHTTPHeadersT headers;
  OZArrayStringT body;
} OZHTTPRequestT;

typedef struct OZHTTPResponse
{
  OZArrayStringT version;
  OZHTTPStatusCodeT code;
  OZHTTPHeadersT headers;
  OZArrayStringT body;
} OZHTTPResponseT;

OZArrayStringT *ozHTTPHeadersGetValue(OZHTTPHeadersT headers, OZArrayStringT key);

OZHTTPHeadersT *ozHTTPHeadersSetHeaders(OZAllocatorT *alloc, OZHTTPHeadersT *headers, OZArrayStringT *keys, OZArrayStringT *values, size_t set_count);

OZHTTPRequestT *ozHTTPRequestCreateFromString(OZAllocatorT *alloc, OZArrayStringT *input);

OZHTTPResponseT *ozHTTPResponseCreate(OZAllocatorT *alloc);

OZArrayStringT *ozHTTPResponseGetString(OZAllocatorT *alloc, OZHTTPResponseT *response);

OZArrayStringT ozHTTPStatusString(OZHTTPStatusCodeT status);

typedef struct OZHTTPHandlerParameter
{
  OZHTTPRequestT *request;
  OZHTTPResponseT *response;
} OZHTTPHandlerParameterT;

int ozHTTPSocketHandlerBegin(OZAllocatorT *alloc, OZSocketHandlerParameterT *param);
int ozHTTPSocketHandlerEnd(OZAllocatorT *alloc, OZSocketHandlerParameterT *param);
void ozHTTPSocketErrorHandler(OZAllocatorT *alloc, OZSocketHandlerParameterT *param, int error);

typedef int(OZHTTPHandlerT)(OZAllocatorT *alloc, OZHTTPHandlerParameterT *param);
int ozHTTPServe(unsigned short int port, OZHTTPHandlerT *handler);

#endif
