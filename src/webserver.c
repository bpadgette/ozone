#include <stdio.h>
#include "allocator.h"

int main(int argc, char **argv)
{
  Allocator *main_alloc = createStrictAllocator();
  if (!main_alloc)
  {
    return 1;
  }

  deleteAllocator(main_alloc);
  return 0;
}
