#include "ozone_file.h"

#include "ozone_log.h"

void ozoneFileLoad(OzoneAllocator* allocator, OzoneStringVector* destination, FILE* file, size_t max_chunk_bytes) {
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

    ozoneVectorPushOzoneString(allocator, destination, string);

    bytes_read += read_status;
    if (read_status != max_chunk_bytes - 1)
      break;
  };

  ozoneLogDebug("Loaded %ld bytes", bytes_read);
}

void ozoneFileLoadFromPath(
    OzoneAllocator* allocator, OzoneStringVector* destination, const OzoneString* path, size_t max_chunk_bytes) {
  FILE* file = fopen(ozoneStringBuffer(path), "r");
  if (!file) {
    ozoneLogError("Could not open file %s", ozoneStringBuffer(path));
    return;
  }

  ozoneLogDebug("Opened file %s", ozoneStringBuffer(path));
  ozoneFileLoad(allocator, destination, file, max_chunk_bytes);
  fclose(file);
}
