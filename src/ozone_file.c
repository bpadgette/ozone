#include "ozone_file.h"

#include "ozone_log.h"

OzoneStringTVectorT ozoneFileLoad(
    OzoneAllocatorT* allocator, FILE* file, OzoneStringEncodingT encoding, size_t max_chunk_bytes) {
  OzoneStringTVectorT vector = (OzoneStringTVectorT) { 0 };
  size_t bytes_read = 0;
  size_t read_status = 0;

  char* cursor = ozoneAllocatorReserveMany(allocator, char, max_chunk_bytes);
  while ((read_status = fread(cursor, 1, max_chunk_bytes - 1, file))) {
    bytes_read += read_status;
    cursor[read_status] = '\0';
    ozoneVectorPushOzoneStringT(allocator, &vector,
        (OzoneStringT) {
            (OzoneVectorCharT) {
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

OzoneStringTVectorT ozoneFileLoadFromPath(
    OzoneAllocatorT* allocator, const OzoneStringT* path, OzoneStringEncodingT encoding, size_t max_chunk_bytes) {
  FILE* file = fopen(ozoneStringBuffer(path), "r");
  if (!file) {
    ozoneLogError("Could not open file %s", ozoneStringBuffer(path));
    return (OzoneStringTVectorT) { 0 };
  }

  ozoneLogDebug("Opened file %s", ozoneStringBuffer(path));
  OzoneStringTVectorT vector = ozoneFileLoad(allocator, file, encoding, max_chunk_bytes);
  fclose(file);

  return vector;
}
