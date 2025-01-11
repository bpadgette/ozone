#ifndef OZONE_TEMPLATE_H
#define OZONE_TEMPLATE_H

#include "ozone_allocator.h"
#include "ozone_string.h"

typedef struct OzoneTemplatesParameterStruct {
  OzoneString name;
} OzoneTemplatesParameter;

typedef struct OzoneTemplatesComponentStruct {
  OzoneString name;
  OzoneStringVector blocks;
  OzoneByteVector block_classes;
} OzoneTemplatesComponent;

#define OZONE_TEMPLATES_BLOCK_CLASS_CONTENT 0
#define OZONE_TEMPLATES_BLOCK_CLASS_NAMED 1

OZONE_VECTOR_DECLARE_API(OzoneTemplatesComponent)

OzoneTemplatesComponent ozoneTemplatesComponentCreate(
    OzoneAllocator* allocator, const OzoneString* component_name, const OzoneStringVector* source);
OzoneTemplatesComponent ozoneTemplatesComponentFromFile(OzoneAllocator* allocator, const OzoneString* source_path);

OzoneString ozoneTemplatesComponentRender(
    OzoneAllocator* allocator, const OzoneTemplatesComponent* component, const OzoneStringKeyValueVector* arguments);

typedef struct OzoneTemplatesConfigStruct {
  OzoneStringKeyValueVector arguments;
  OzoneTemplatesComponentVector components;
} OzoneTemplatesConfig;

OzoneString
ozoneTemplatesRender(OzoneAllocator* allocator, OzoneTemplatesConfig* config, const OzoneString* component_name);

#endif
