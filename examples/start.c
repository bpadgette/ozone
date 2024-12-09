#include "ozone.h"

int handler(OzoneAllocatorT *alloc, OzoneHTTPHandlerParameterT *param)
{
  (void)alloc;

  param->response->body = ozoneArrayStringFromChars("Hello, world!");

  return 0;
}

int main()
{
  return ozoneHTTPServe(8080, handler);
}
