#include <stdio.h>
#include "allocator.h"

#define MAX_RESPONSE_SIZE 1024

int main(int argc, char **argv)
{
  Allocator *main_alloc = createStrictAllocator();
  if (!main_alloc)
  {
    return 1;
  }

  char *response = (char *)pushAllocator(main_alloc, MAX_RESPONSE_SIZE);
  snprintf(response, MAX_RESPONSE_SIZE, "hello world");
  printf("%s\n", response);

  deleteAllocator(main_alloc);
  return 0;
}
