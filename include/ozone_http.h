#ifndef OZONE_HTTP
#define OZONE_HTTP

#include "ozone_allocator.h"
#include "ozone_map.h"
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
OzoneString* ozoneHTTPStatusText(OzoneAllocator* allocator, OzoneHTTPStatusCode status);

typedef struct OzoneHTTPRequestStruct {
  OzoneHTTPMethod method;
  OzoneString target;
  OzoneHTTPVersion version;
  OzoneStringMap headers;
  OzoneString body;
} OzoneHTTPRequest;

typedef struct OzoneHTTPResponseStruct {
  OzoneHTTPVersion version;
  OzoneHTTPStatusCode code;
  OzoneStringMap headers;
  OzoneString body;
} OzoneHTTPResponse;

typedef struct OzoneHTTPEventStruct OZONE_SOCKET_EVENT_FIELDS(OzoneHTTPRequest, OzoneHTTPResponse, void) OzoneHTTPEvent;

typedef void(OzoneHTTPHandler)(OzoneHTTPEvent* event);

OzoneHTTPRequest* ozoneHTTPParseSocketRequest(OzoneAllocator* allocator, const OzoneStringVector* socket_request);
OzoneStringVector* ozoneHTTPRenderResponse(OzoneAllocator* allocator, OzoneHTTPResponse* http_response);

typedef struct OzoneHTTPConfigStruct {
  unsigned short int port;
  OzoneSocketHandlerRefVector handler_pipeline;
  void* handler_context;
} OzoneHTTPConfig;

int ozoneHTTPServe(OzoneHTTPConfig* config);

#endif
