#ifndef OZONE_HTTP
#define OZONE_HTTP
#include "ozone_allocator.h"
#include "ozone_socket.h"
#include "ozone_string.h"

typedef enum OzoneHTTPVersion {
  OZONE_HTTP_VERSION_UNKNOWN,
  OZONE_HTTP_VERSION_1_0 = 10,
  OZONE_HTTP_VERSION_1_1 = 11,
} OzoneHTTPVersionT;

OzoneHTTPVersionT ozoneHTTPParseVersion(const OzoneStringT* version_string);

typedef enum OzoneHTTPMethod {
  OZONE_HTTP_METHOD_UNKNOWN,
  OZONE_HTTP_METHOD_GET,
  OZONE_HTTP_METHOD_HEAD,
  OZONE_HTTP_METHOD_POST,
  OZONE_HTTP_METHOD_PUT,
  OZONE_HTTP_METHOD_DELETE,
  OZONE_HTTP_METHOD_CONNECT,
  OZONE_HTTP_METHOD_OPTIONS,
  OZONE_HTTP_METHOD_TRACE,
  OZONE_HTTP_METHOD_PATCH,
} OzoneHTTPMethodT;

OzoneHTTPMethodT ozoneHTTPParseMethod(const OzoneStringT* method_string);

typedef unsigned short int OzoneHTTPStatusCodeT;

OzoneStringT ozoneHTTPStatusString(OzoneHTTPStatusCodeT status);

typedef struct OzoneHTTPHeader {
  OzoneStringT name;
  OzoneStringT value;
  struct OzoneHTTPHeader* next;
} OzoneHTTPHeaderT;

OzoneStringT* ozoneHTTPGetHeaderValue(OzoneHTTPHeaderT* headers, OzoneStringT name);
void ozoneHTTPAppendHeader(
    OzoneAllocatorT* allocator, OzoneHTTPHeaderT** headers, OzoneStringT name, OzoneStringT value);

typedef struct OzoneHTTPRequest {
  OzoneHTTPMethodT method;
  OzoneStringT target;
  OzoneHTTPVersionT version;
  OzoneHTTPHeaderT* headers;
  OzoneStringT body;
} OzoneHTTPRequestT;

typedef struct OzoneHTTPResponse {
  OzoneHTTPVersionT version;
  OzoneHTTPStatusCodeT code;
  OzoneHTTPHeaderT* headers;
  OzoneStringT body;
} OzoneHTTPResponseT;

typedef struct OzoneHTTPHandlerContext {
  OzoneHTTPRequestT request;
  OzoneHTTPResponseT response;
} OzoneHTTPHandlerContextT;

typedef struct OzoneHTTPContext OZONE_SOCKET_HANDLER_CONTEXT_FIELDS(void, OzoneHTTPHandlerContextT) OzoneHTTPContextT;

typedef int(OzoneHTTPHandlerT)(OzoneHTTPContextT* context);

OzoneHTTPRequestT* ozoneHTTPParseSocketChunks(OzoneAllocatorT* allocator, const OzoneSocketChunkT* socket_request);
OzoneSocketChunkT* ozoneHTTPCreateSocketChunks(OzoneAllocatorT* allocator, OzoneHTTPResponseT* http_response);

typedef struct OzoneHTTPConfig {
  unsigned short int port;
  OzoneHTTPHandlerT** handler_pipeline;
  size_t handler_pipeline_count;
  void* application_context;
} OzoneHTTPConfigT;

int ozoneHTTPServe(OzoneAllocatorT* allocator, OzoneHTTPConfigT config);

#endif
