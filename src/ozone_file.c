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

    OzoneString* string = ozoneAllocatorReserveOne(allocator, OzoneString);
    string->vector.capacity = read_status + 1;
    string->vector.length = read_status + 1;
    string->vector.elements = cursor;

    OzoneStringVectorPush(allocator, destination, string);

    bytes_read += read_status;
    if (read_status != max_chunk_bytes - 1)
      break;
  };

  (void)bytes_read;
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
