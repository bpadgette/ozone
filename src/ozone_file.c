#include "ozone_file.h"

#include "ozone_log.h"

OzoneStringVector ozoneFileLoad(OzoneAllocator* allocator, FILE* file, size_t max_chunk_bytes) {
  OzoneStringVector vector = (OzoneStringVector) { 0 };
  size_t bytes_read = 0;

  for (;;) {
    char* cursor = ozoneAllocatorReserveMany(allocator, char, max_chunk_bytes);
    size_t read_status = fread(cursor, 1, max_chunk_bytes - 1, file);
    if (!read_status)
      break;

    cursor[read_status] = '\0';
    pushOzoneString(
        allocator,
        &vector,
        (OzoneString) {
            (OzoneByteVector) {
                .elements = cursor,
                .length = read_status + 1,
                .capacity = read_status + 1,
            },
        });

    bytes_read += read_status;
    if (read_status != max_chunk_bytes - 1)
      break;
  };

  ozoneLogDebug("Loaded %ld bytes into %ld chunks", bytes_read, ozoneVectorLength(&vector));

  return vector;
}

OzoneStringVector ozoneFileLoadFromPath(OzoneAllocator* allocator, const OzoneString* path, size_t max_chunk_bytes) {
  FILE* file = fopen(ozoneStringBuffer(path), "r");
  if (!file) {
    ozoneLogError("Could not open file %s", ozoneStringBuffer(path));
    return (OzoneStringVector) { 0 };
  }

  ozoneLogDebug("Opened file %s", ozoneStringBuffer(path));
  OzoneStringVector vector = ozoneFileLoad(allocator, file, max_chunk_bytes);
  fclose(file);

  return vector;
}
