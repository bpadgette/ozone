#include "ozone_file.h"

#include "ozone_log.h"

OzoneStringVector ozoneFileLoad(
    OzoneAllocator* allocator, FILE* file, OzoneStringEncoding encoding, size_t max_chunk_bytes) {
  OzoneStringVector vector = (OzoneStringVector) { 0 };
  size_t bytes_read = 0;
  size_t read_status = 0;

  char* cursor = ozoneAllocatorReserveMany(allocator, char, max_chunk_bytes);
  while ((read_status = fread(cursor, 1, max_chunk_bytes - 1, file))) {
    bytes_read += read_status;
    cursor[read_status] = '\0';
    ozoneVectorPushOzoneString(allocator, &vector,
        (OzoneString) {
            (OzoneVectorChar) {
                .elements = cursor,
                .length = read_status + 1,
                .capacity = read_status + 1,
                .capacity_increment = read_status + 1,
            },
            .encoding = encoding,
        });

    cursor = ozoneAllocatorReserveMany(allocator, char, max_chunk_bytes);
  };

  ozoneLogDebug("Loaded %ld bytes", bytes_read);

  return vector;
}

OzoneStringVector ozoneFileLoadFromPath(
    OzoneAllocator* allocator, const OzoneString* path, OzoneStringEncoding encoding, size_t max_chunk_bytes) {
  FILE* file = fopen(ozoneStringBuffer(path), "r");
  if (!file) {
    ozoneLogError("Could not open file %s", ozoneStringBuffer(path));
    return (OzoneStringVector) { 0 };
  }

  ozoneLogDebug("Opened file %s", ozoneStringBuffer(path));
  OzoneStringVector vector = ozoneFileLoad(allocator, file, encoding, max_chunk_bytes);
  fclose(file);

  return vector;
}
