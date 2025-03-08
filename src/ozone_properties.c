#include "ozone_properties.h"

#define OZONE_PROPERTIES_PARSING_WHITESPACE 0
#define OZONE_PROPERTIES_PARSING_COMMENTS 1
#define OZONE_PROPERTIES_PARSING_KEY 2
#define OZONE_PROPERTIES_PARSING_KEY_ESCAPED 3
#define OZONE_PROPERTIES_PARSING_KEY_WHITESPACE 4
#define OZONE_PROPERTIES_PARSING_VALUE 5
#define OZONE_PROPERTIES_PARSING_VALUE_ESCAPED 6
#define OZONE_PROPERTIES_PARSING_VALUE_WHITESPACE 7

void ozonePropertiesLoadFromText(OzoneAllocator* allocator, OzoneStringMap* properties, const OzoneStringVector* text) {
  int parsing = OZONE_PROPERTIES_PARSING_WHITESPACE;
  OzoneString* key = ozoneStringAllocate(allocator, "");
  OzoneString* value = ozoneStringAllocate(allocator, "");
  OzoneString* chunk;
  ozoneVectorForEach(chunk, text) {
    char cursor = '\0';
    for (size_t cursor_index = 0; cursor_index < ozoneStringLength(chunk); cursor_index++) {
      cursor = ozoneStringBufferAt(chunk, cursor_index);
      switch (parsing) {
      case OZONE_PROPERTIES_PARSING_WHITESPACE:
        if (cursor == '#' || cursor == '!')
          parsing = OZONE_PROPERTIES_PARSING_COMMENTS;
        else if (cursor > ' ') {
          ozoneStringWriteByte(allocator, key, cursor);
          parsing = OZONE_PROPERTIES_PARSING_KEY;
        }

        break;
      case OZONE_PROPERTIES_PARSING_COMMENTS:
        if (cursor == '\n')
          parsing = OZONE_PROPERTIES_PARSING_WHITESPACE;

        break;
      case OZONE_PROPERTIES_PARSING_KEY:
        if (cursor == '\\')
          parsing = OZONE_PROPERTIES_PARSING_KEY_ESCAPED;
        else if (cursor == ' ')
          parsing = OZONE_PROPERTIES_PARSING_KEY_WHITESPACE;
        else if (cursor == '=' || cursor == ':')
          parsing = OZONE_PROPERTIES_PARSING_VALUE_WHITESPACE;
        else if (cursor == '\n') {
          if (ozoneStringLength(key)) {
            OzoneStringMapInsert(allocator, properties, key, ozoneStringAllocate(allocator, ""));
            ozoneStringClear(key);
          }
          parsing = OZONE_PROPERTIES_PARSING_WHITESPACE;
        } else if (cursor > ' ' && cursor < 127)
          ozoneStringWriteByte(allocator, key, cursor);

        break;
      case OZONE_PROPERTIES_PARSING_KEY_ESCAPED:
        if (cursor >= ' ' && cursor < 127)
          ozoneStringWriteByte(allocator, key, cursor);

        parsing = OZONE_PROPERTIES_PARSING_KEY;

        break;
      case OZONE_PROPERTIES_PARSING_KEY_WHITESPACE:
        if (cursor == '=' || cursor == ':')
          parsing = OZONE_PROPERTIES_PARSING_VALUE_WHITESPACE;
        else if (cursor > ' ' && cursor < 127) {
          ozoneStringWriteByte(allocator, value, cursor);
          parsing = OZONE_PROPERTIES_PARSING_VALUE;
        }

        break;
      case OZONE_PROPERTIES_PARSING_VALUE:
        if (cursor == '\\')
          parsing = OZONE_PROPERTIES_PARSING_VALUE_ESCAPED;
        else if (cursor == '\n') {
          if (ozoneStringLength(key)) {
            OzoneStringMapInsert(allocator, properties, key, ozoneStringCopy(allocator, value));
          }

          ozoneStringClear(key);
          ozoneStringClear(value);

          parsing = OZONE_PROPERTIES_PARSING_WHITESPACE;
        } else if (cursor >= ' ' && cursor < 127)
          ozoneStringWriteByte(allocator, value, cursor);

        break;
      case OZONE_PROPERTIES_PARSING_VALUE_ESCAPED:
        if (cursor == '\n')
          parsing = OZONE_PROPERTIES_PARSING_VALUE_WHITESPACE;
        else if (cursor == 'n')
          ozoneStringWriteByte(allocator, value, '\n');
        else if (cursor == 'r')
          ozoneStringWriteByte(allocator, value, '\r');
        else if (cursor == 't')
          ozoneStringWriteByte(allocator, value, '\t');
        else if (cursor >= ' ' && cursor < 127) {
          ozoneStringWriteByte(allocator, value, cursor);
          parsing = OZONE_PROPERTIES_PARSING_VALUE;
        } else
          parsing = OZONE_PROPERTIES_PARSING_VALUE;

        break;
      case OZONE_PROPERTIES_PARSING_VALUE_WHITESPACE:
        if (cursor == '\\')
          parsing = OZONE_PROPERTIES_PARSING_VALUE_ESCAPED;
        else if (cursor > ' ' && cursor < 127) {
          ozoneStringWriteByte(allocator, value, cursor);
          parsing = OZONE_PROPERTIES_PARSING_VALUE;
        }
        break;
      }
    }
  }

  if (ozoneStringLength(key)) {
    OzoneStringMapInsert(allocator, properties, key, ozoneStringCopy(allocator, value));
  }
}

void ozonePropertiesLoadFromPath(OzoneAllocator* allocator, OzoneStringMap* properties, const OzoneString* path) {
  OzoneStringVector text = (OzoneStringVector) { 0 };
  ozoneFileLoadFromPath(allocator, &text, path, 1024);
  ozonePropertiesLoadFromText(allocator, properties, &text);
}
