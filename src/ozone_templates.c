#include "ozone_templates.h"

#include "ozone_file.h"

#define OZONE_TEMPLATES_PARSING_CONTENT 1
#define OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE 2
#define OZONE_TEMPLATES_PARSING_PARAMETER_NAME 3
#define OZONE_TEMPLATES_PARSING_PARAMETER_WHITESPACE_AFTER_NAME 4

OZONE_VECTOR_IMPLEMENT_API(OzoneTemplatesComponent)

OzoneTemplatesComponent* ozoneTemplatesComponentCreate(
    OzoneAllocator* allocator, const OzoneString* component_name, const OzoneStringVector* source) {
  OzoneStringVector blocks = (OzoneStringVector) { 0 };
  OzoneString block_classes = (OzoneString) { 0 };

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
          ozoneVectorPushOzoneString(allocator, &blocks, ozoneStringCopy(allocator, token));
          ozoneStringAppend(allocator, &block_classes, OZONE_TEMPLATES_BLOCK_CLASS_CONTENT);
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
          ozoneVectorPushOzoneString(allocator, &blocks, ozoneStringCopy(allocator, token));
          ozoneStringAppend(allocator, &block_classes, OZONE_TEMPLATES_BLOCK_CLASS_NAMED);
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
    ozoneVectorPushOzoneString(allocator, &blocks, token);
    ozoneStringAppend(allocator, &block_classes, OZONE_TEMPLATES_BLOCK_CLASS_CONTENT);
  }

  OzoneTemplatesComponent* component = ozoneAllocatorReserveOne(allocator, OzoneTemplatesComponent);
  *component = (OzoneTemplatesComponent) {
    .name = *ozoneStringCopy(allocator, component_name),
    .blocks = blocks,
    .block_classes = block_classes,
  };

  return component;
}

OzoneTemplatesComponent* ozoneTemplatesComponentFromFile(OzoneAllocator* allocator, const OzoneString* source_path) {
  OzoneStringVector* source = ozoneAllocatorReserveOne(allocator, OzoneStringVector);
  ozoneFileLoadFromPath(allocator, source, source_path, 256);
  return ozoneTemplatesComponentCreate(allocator, source_path, source);
}

OzoneString* ozoneTemplatesComponentRender(
    OzoneAllocator* allocator, const OzoneTemplatesComponent* component, const OzoneStringMap* arguments) {
  OzoneString* rendered = ozoneString(allocator, "");
  OzoneString* block;
  ozoneVectorForEach(block, &component->blocks) {
    switch (ozoneStringBufferAt(&component->block_classes, ozoneVectorIndex(&component->blocks, block))) {
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
