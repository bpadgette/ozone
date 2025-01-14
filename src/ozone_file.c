#include "ozone_file.h"

#include "ozone_log.h"

OzoneStringVector* ozoneFileLoad(OzoneAllocator* allocator, FILE* file, size_t max_chunk_bytes) {
  OzoneStringVector* vector = ozoneAllocatorReserveOne(allocator, OzoneStringVector);
  size_t bytes_read = 0;

  for (;;) {
    char* cursor = ozoneAllocatorReserveMany(allocator, char, max_chunk_bytes);
    size_t read_status = fread(cursor, 1, max_chunk_bytes - 1, file);
    if (!read_status)
      break;

    cursor[read_status] = '\0';

    OzoneByteVector* byte_vector = ozoneAllocatorReserveOne(allocator, OzoneByteVector);
    byte_vector->capacity = read_status + 1;
    byte_vector->length = read_status + 1;
    byte_vector->elements = cursor;

    OzoneString* string = ozoneAllocatorReserveOne(allocator, OzoneString);
    string->vector = *byte_vector;

    ozoneVectorPushOzoneString(allocator, vector, string);

    bytes_read += read_status;
    if (read_status != max_chunk_bytes - 1)
      break;
  };

  ozoneLogDebug("Loaded %ld bytes into %ld chunks", bytes_read, ozoneVectorLength(vector));

  return vector;
}

OzoneStringVector* ozoneFileLoadFromPath(OzoneAllocator* allocator, const OzoneString* path, size_t max_chunk_bytes) {
  FILE* file = fopen(ozoneStringBuffer(path), "r");
  if (!file) {
    ozoneLogError("Could not open file %s", ozoneStringBuffer(path));
    return NULL;
  }

  ozoneLogDebug("Opened file %s", ozoneStringBuffer(path));
  OzoneStringVector* vector = ozoneFileLoad(allocator, file, max_chunk_bytes);
  fclose(file);

  return vector;
}
