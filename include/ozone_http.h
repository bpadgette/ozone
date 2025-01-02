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
} OzoneHTTPHeaderT;

OZONE_VECTOR_DECLARE_API(HTTPHeaderT)
#define ozoneHTTPAppendHeader(_allocator_, _headers_, _name_, _value_)                                                 \
  ozoneHTTPHeaderTVectorPush(_allocator_, _headers_,                                                                   \
      (OzoneHTTPHeaderT) {                                                                                             \
          .name = _name_,                                                                                              \
          .value = _value_,                                                                                            \
      });

OzoneStringT* ozoneHTTPGetHeaderValue(OzoneHTTPHeaderTVectorT* headers, OzoneStringT name);

typedef struct OzoneHTTPRequest {
  OzoneHTTPMethodT method;
  OzoneStringT target;
  OzoneHTTPVersionT version;
  OzoneHTTPHeaderTVectorT headers;
  OzoneStringT body;
} OzoneHTTPRequestT;

typedef struct OzoneHTTPResponse {
  OzoneHTTPVersionT version;
  OzoneHTTPStatusCodeT code;
  OzoneHTTPHeaderTVectorT headers;
  OzoneStringT body;
} OzoneHTTPResponseT;

typedef struct OzoneHTTPContext OZONE_SOCKET_CONTEXT_FIELDS(
    OzoneHTTPRequestT, OzoneHTTPResponseT, void) OzoneHTTPContextT;

typedef int(OzoneHTTPHandlerT)(OzoneHTTPContextT* context);

OzoneHTTPRequestT* ozoneHTTPParseSocketChunks(OzoneAllocatorT* allocator, const OzoneSocketChunkT* socket_request);
OzoneSocketChunkT* ozoneHTTPCreateSocketChunks(OzoneAllocatorT* allocator, OzoneHTTPResponseT* http_response);

typedef struct OzoneHTTPConfig {
  OzoneAllocatorT* allocator;
  unsigned short int port;
  OzoneHTTPHandlerT** handler_pipeline;
  size_t handler_pipeline_count;
  void* application;
} OzoneHTTPConfigT;

int ozoneHTTPServe(OzoneHTTPConfigT config);

#endif
