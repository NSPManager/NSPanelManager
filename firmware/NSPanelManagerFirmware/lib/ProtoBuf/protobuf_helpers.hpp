#ifndef PROTOBUF_HELPERS_H

#include <Arduino.h>
#include <protobuf_nspanel.pb.h>

class ProtobufHelpers {
public:
  static bool write_string(pb_ostream_t *stream, const pb_field_iter_t *field, void *const *arg);
};

#endif