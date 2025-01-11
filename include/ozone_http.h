#ifndef OZONE_HTTP
#define OZONE_HTTP

#include "ozone_allocator.h"
#include "ozone_socket.h"
#include "ozone_string.h"

typedef enum OzoneHTTPVersion {
  OZONE_HTTP_VERSION_UNKNOWN,
  OZONE_HTTP_VERSION_1_0 = 10,
  OZONE_HTTP_VERSION_1_1 = 11,
} OzoneHTTPVersion;

OzoneHTTPVersion ozoneHTTPParseVersion(const OzoneString* version_string);

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
} OzoneHTTPMethod;

OzoneHTTPMethod ozoneHTTPParseMethod(const OzoneString* method_string);

typedef unsigned short int OzoneHTTPStatusCode;
OzoneString ozoneHTTPStatusString(OzoneHTTPStatusCode status);

typedef struct OzoneHTTPRequestStruct {
  OzoneHTTPMethod method;
  OzoneString target;
  OzoneHTTPVersion version;
  OzoneStringKeyValueVector headers;
  OzoneString body;
} OzoneHTTPRequest;

typedef struct OzoneHTTPResponseStruct {
  OzoneHTTPVersion version;
  OzoneHTTPStatusCode code;
  OzoneStringKeyValueVector headers;
  OzoneString body;
} OzoneHTTPResponse;

typedef struct OzoneHTTPEventStruct OZONE_SOCKET_EVENT_FIELDS(OzoneHTTPRequest, OzoneHTTPResponse) OzoneHTTPEvent;

typedef int(OzoneHTTPHandler)(OzoneHTTPEvent* event, void* application);

OzoneHTTPRequest* ozoneHTTPParseSocketChunks(OzoneAllocator* allocator, const OzoneSocketChunk* socket_request);
OzoneSocketChunk* ozoneHTTPCreateSocketChunks(OzoneAllocator* allocator, OzoneHTTPResponse* http_response);

typedef struct OzoneHTTPConfigStruct {
  unsigned short int port;
  OzoneSocketHandlerRefVector handler_pipeline;
  void* handler_context;
} OzoneHTTPConfig;

int ozoneHTTPServe(OzoneAllocator* allocator, OzoneHTTPConfig config);

#endif
