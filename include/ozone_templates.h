#ifndef OZONE_TEMPLATE_H
#define OZONE_TEMPLATE_H

#include "ozone_allocator.h"
#include "ozone_map.h"
#include "ozone_string.h"

typedef struct OzoneTemplatesComponentStruct {
  OzoneString name;
  OzoneStringVector blocks;
  OzoneString block_classes;
} OzoneTemplatesComponent;

#define OZONE_TEMPLATES_BLOCK_CLASS_CONTENT 1
#define OZONE_TEMPLATES_BLOCK_CLASS_NAMED 2

OZONE_VECTOR_DECLARE_API(OzoneTemplatesComponent)

OzoneTemplatesComponent* ozoneTemplatesComponentCreate(
    OzoneAllocator* allocator, const OzoneString* component_path, const OzoneStringVector* source);
OzoneTemplatesComponent* ozoneTemplatesComponentFromFile(OzoneAllocator* allocator, const OzoneString* source_path);

OzoneString* ozoneTemplatesComponentRender(
    OzoneAllocator* allocator, const OzoneTemplatesComponent* component, OzoneStringMap* arguments);

typedef struct OzoneTemplatesConfigStruct {
  OzoneTemplatesComponentVector components;
} OzoneTemplatesConfig;

#endif
