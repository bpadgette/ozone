#ifndef OZONE_TEMPLATE_H
#define OZONE_TEMPLATE_H

#include "ozone_allocator.h"
#include "ozone_map.h"
#include "ozone_string.h"

typedef struct OzoneTemplateStruct {
  OzoneString name;
  OzoneStringVector blocks;
  OzoneString block_classes;
} OzoneTemplate;

#define OZONE_TEMPLATES_BLOCK_CLASS_CONTENT 1
#define OZONE_TEMPLATES_BLOCK_CLASS_NAMED 2

OZONE_VECTOR_DECLARE_API(OzoneTemplate)

OzoneTemplate*
ozoneTemplateCreate(OzoneAllocator* allocator, const OzoneString* component_path, const OzoneStringVector* source);
OzoneTemplate* ozoneTemplateFromFile(OzoneAllocator* allocator, const OzoneString* source_path);

#define ozoneTemplate(_allocator_, _chars_)                                                                            \
  ozoneTemplateCreate(                                                                                                 \
      _allocator_, &ozoneString("ozone_template:local"), &ozoneVector(OzoneString, ozoneString(_chars_)))

void ozoneTemplateWrite(
    OzoneAllocator* allocator, OzoneString* destination, const OzoneTemplate* component, OzoneStringMap* arguments);

typedef struct OzoneTemplateConfigStruct {
  OzoneTemplateVector components;
} OzoneTemplateConfig;

#endif
