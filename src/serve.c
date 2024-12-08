#include "http.h"

int handler(OZAllocatorT *alloc, OZHTTPHandlerParameterT *param)
{
  (void)alloc;

  param->response->body = ozArrayStringFromChars("Hello, world!");

  return 0;
}

int main()
{
  return ozHTTPServe(8080, handler);
}
