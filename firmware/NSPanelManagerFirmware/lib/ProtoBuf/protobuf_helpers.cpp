// #include <MqttLog.hpp>
// #include <pb_decode.h>
// #include <pb_encode.h>
// #include <protobuf_helpers.hpp>

// bool ProtobufHelpers::write_string(pb_ostream_t *stream, const pb_field_t *field, void *const *arg) {
//   const char *str = (const char *)(*arg);

//   if (!pb_encode_tag_for_field(stream, field)) {
//     return false;
//   }

//   return pb_encode_string(stream, (uint8_t *)str, strlen(str));
// }

// // Callback function to read the string
// bool ProtobufHelpers::read_string(pb_istream_t *stream, const pb_field_t *field, void **arg) {
//   std::string *str = static_cast<std::string *>(*arg);
//   str->resize(stream->bytes_left);
//   return pb_read(stream, reinterpret_cast<pb_byte_t *>(&(*str)[0]), stream->bytes_left);
// }