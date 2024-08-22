#include <pb_encode.h>
#include <protobuf_helpers.hpp>

bool ProtobufHelpers::write_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg) {
  const char *str = (const char *)(*arg);

  if (!pb_encode_tag_for_field(stream, field))
    return false;

  return pb_encode_string(stream, (uint8_t *)str, strlen(str));
}