#ifndef OZONE_HTTP
#define OZONE_HTTP

#include "ozone_allocator.h"
#include "ozone_socket.h"
#include "ozone_string.h"
#include "ozone_vector.h"

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

typedef struct OzoneHTTPHeaderStruct {
  OzoneString name;
  OzoneString value;
} OzoneHTTPHeader;

OZONE_VECTOR_DECLARE_API(OzoneHTTPHeader)
#define ozoneHTTPAppendHeader(_allocator_, _headers_, _name_, _value_)                                                 \
  ozoneVectorPushOzoneHTTPHeader(_allocator_, _headers_,                                                               \
      ((OzoneHTTPHeader) {                                                                                             \
          .name = ozoneStringCopy(_allocator_, _name_),                                                                \
          .value = ozoneStringCopy(_allocator_, _value_),                                                              \
      }));

OzoneString* ozoneHTTPGetHeaderValue(OzoneHTTPHeaderVector* headers, OzoneString name);

typedef struct OzoneHTTPRequestStruct {
  OzoneHTTPMethod method;
  OzoneString target;
  OzoneHTTPVersion version;
  OzoneHTTPHeaderVector headers;
  OzoneString body;
} OzoneHTTPRequest;

typedef struct OzoneHTTPResponseStruct {
  OzoneHTTPVersion version;
  OzoneHTTPStatusCode code;
  OzoneHTTPHeaderVector headers;
  OzoneString body;
} OzoneHTTPResponse;

typedef struct OzoneHTTPContextStruct OZONE_SOCKET_CONTEXT_FIELDS(
    OzoneHTTPRequest, OzoneHTTPResponse, void) OzoneHTTPContext;

typedef int(OzoneHTTPHandler)(OzoneHTTPContext* context);

OzoneHTTPRequest* ozoneHTTPParseSocketChunks(OzoneAllocator* allocator, const OzoneSocketChunk* socket_request);
OzoneSocketChunk* ozoneHTTPCreateSocketChunks(OzoneAllocator* allocator, OzoneHTTPResponse* http_response);

typedef struct OzoneHTTPConfigStruct {
  OzoneAllocator* allocator;
  unsigned short int port;
  OzoneHTTPHandler** handler_pipeline;
  size_t handler_pipeline_count;
  void* application;
} OzoneHTTPConfig;

int ozoneHTTPServe(OzoneHTTPConfig config);

#endif
