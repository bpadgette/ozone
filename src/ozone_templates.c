#include "ozone_templates.h"

#include "ozone_file.h"

#define OZONE_TEMPLATES_PARSING_CONTENT 1
#define OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE 2
#define OZONE_TEMPLATES_PARSING_PARAMETER_NAME 3
#define OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE_AFTER_NAME 4

OZONE_VECTOR_IMPLEMENT_API(OzoneTemplatesComponent)

OzoneTemplatesComponent* ozoneTemplatesComponentCreate(
    OzoneAllocator* allocator, const OzoneString* component_name, const OzoneStringVector* source) {
  OzoneTemplatesComponent* component = ozoneAllocatorReserveOne(allocator, OzoneTemplatesComponent);
  component->name = *ozoneStringCopy(allocator, component_name);

  OzoneString* token = ozoneString(allocator, "");

  int parsing = OZONE_TEMPLATES_PARSING_CONTENT;
  OzoneString* source_chunk;
  ozoneVectorForEach(source_chunk, source) {
    for (size_t cursor_index = 0; cursor_index < ozoneStringLength(source_chunk); cursor_index++) {
      char cursor = ozoneStringBufferAt(source_chunk, cursor_index);
      switch (parsing) {
      case OZONE_TEMPLATES_PARSING_CONTENT: {
        if (cursor == '{' && ozoneStringBufferEnd(token) == '{') {
          ozoneStringPop(token);
          ozoneVectorPushOzoneString(allocator, &component->blocks, ozoneStringCopy(allocator, token));
          ozoneStringAppend(allocator, &component->block_classes, OZONE_TEMPLATES_BLOCK_CLASS_CONTENT);
          ozoneStringClear(token);
          parsing = OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE;
        } else {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      case OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE: {
        if (cursor > ' ') {
          ozoneStringAppend(allocator, token, cursor);
          parsing = OZONE_TEMPLATES_PARSING_PARAMETER_NAME;
        }
        break;
      }
      case OZONE_TEMPLATES_PARSING_PARAMETER_NAME: {
        if (cursor == '}') {
          ozoneStringAppend(allocator, token, cursor);
          parsing = OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE_AFTER_NAME;
        } else if (cursor <= ' ') {
          parsing = OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE_AFTER_NAME;
        } else {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      case OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE_AFTER_NAME: {
        if (cursor == '}' && ozoneStringBufferEnd(token) == '}') {
          ozoneStringPop(token);
          ozoneVectorPushOzoneString(allocator, &component->blocks, ozoneStringCopy(allocator, token));
          ozoneStringAppend(allocator, &component->block_classes, OZONE_TEMPLATES_BLOCK_CLASS_NAMED);
          ozoneStringClear(token);
          parsing = OZONE_TEMPLATES_PARSING_CONTENT;
        } else if (cursor == '}') {
          ozoneStringAppend(allocator, token, cursor);
        }
        break;
      }
      }
    }
  }

  if (ozoneStringLength(token)) {
    ozoneVectorPushOzoneString(allocator, &component->blocks, token);
    ozoneStringAppend(allocator, &component->block_classes, OZONE_TEMPLATES_BLOCK_CLASS_CONTENT);
  }

  return component;
}

OzoneTemplatesComponent* ozoneTemplatesComponentFromFile(OzoneAllocator* allocator, const OzoneString* source_path) {
  OzoneStringVector source = (OzoneStringVector) { 0 };
  ozoneFileLoadFromPath(allocator, &source, source_path, 256);
  return ozoneTemplatesComponentCreate(allocator, source_path, &source);
}

OzoneString* ozoneTemplatesComponentRender(
    OzoneAllocator* allocator, const OzoneTemplatesComponent* component, const OzoneStringMap* arguments) {
  OzoneString* rendered = ozoneString(allocator, "");

  for (size_t block_index = 0; block_index < ozoneVectorLength(&component->blocks); block_index++) {
    OzoneString* block = &ozoneVectorAt(&component->blocks, block_index);
    char block_class = block_index < ozoneStringLength(&component->block_classes)
        ? ozoneStringBufferAt(&component->block_classes, block_index)
        : OZONE_TEMPLATES_BLOCK_CLASS_CONTENT;

    switch (block_class) {
    case OZONE_TEMPLATES_BLOCK_CLASS_CONTENT: {
      ozoneStringConcatenate(allocator, rendered, block);
      break;
    }
    case OZONE_TEMPLATES_BLOCK_CLASS_NAMED: {
      if (!arguments)
        break;

      const OzoneString* argument = ozoneStringMapFindValue(arguments, block);
      if (!argument)
        break;

      ozoneStringConcatenate(allocator, rendered, argument);
      break;
    }
    }
  }

  return rendered;
}
