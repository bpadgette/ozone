#ifndef OZONE_APP_H
#define OZONE_APP_H

#include "ozone_allocator.h"
#include "ozone_http.h"
#include "ozone_router.h"
#include "ozone_socket.h"
#include "ozone_string.h"

typedef struct OzoneAppConfig {
  OzoneAllocatorT* allocator;
  unsigned short int port;
  OzoneRouterConfigT router;
} OzoneAppConfigT;

int ozoneAppServe(OzoneAppConfigT config);

#endif
