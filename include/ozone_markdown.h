#ifndef OZONE_MARKDOWN_H
#define OZONE_MARKDOWN_H

#include "ozone_string.h"
#include "ozone_vector.h"

typedef struct OzoneMarkdownBlockStruct {
  OzoneString text;
  SizeTVector child_indexes;
} OzoneMarkdownBlock;

OZONE_VECTOR_DECLARE_API(OzoneMarkdownBlock)

#endif
