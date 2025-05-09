/*
 *  This file is generated with Embedded Proto, PLEASE DO NOT EDIT!
 *  source: protobuf_nspanel.proto
 */

// This file is generated. Please do not edit!
#ifndef PROTOBUF_NSPANEL_H
#define PROTOBUF_NSPANEL_H

#include <cstdint>
#include <MessageInterface.h>
#include <WireFormatter.h>
#include <Fields.h>
#include <MessageSizeCalculator.h>
#include <ReadBufferSection.h>
#include <RepeatedFieldFixedSize.h>
#include <FieldStringBytes.h>
#include <Errors.h>
#include <Defines.h>
#include <limits>

// Include external proto definitions


enum class NSPanelWarningLevel : uint32_t
{
  CRITICAL = 0,
  ERROR = 1,
  WARNING = 2,
  INFO = 3,
  DEBUG = 4,
  TRACE = 5
};

template<
    uint32_t NSPanelLightStatus_name_LENGTH
>
class NSPanelLightStatus final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelLightStatus() = default;
    NSPanelLightStatus(const NSPanelLightStatus& rhs )
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_can_dim(rhs.get_can_dim());
      set_can_color_temperature(rhs.get_can_color_temperature());
      set_can_rgb(rhs.get_can_rgb());
      set_light_level(rhs.get_light_level());
      set_color_temp(rhs.get_color_temp());
      set_hue(rhs.get_hue());
      set_saturation(rhs.get_saturation());
      set_room_view_position(rhs.get_room_view_position());
    }

    NSPanelLightStatus(const NSPanelLightStatus&& rhs ) noexcept
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_can_dim(rhs.get_can_dim());
      set_can_color_temperature(rhs.get_can_color_temperature());
      set_can_rgb(rhs.get_can_rgb());
      set_light_level(rhs.get_light_level());
      set_color_temp(rhs.get_color_temp());
      set_hue(rhs.get_hue());
      set_saturation(rhs.get_saturation());
      set_room_view_position(rhs.get_room_view_position());
    }

    ~NSPanelLightStatus() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      ID = 1,
      NAME = 2,
      CAN_DIM = 3,
      CAN_COLOR_TEMPERATURE = 4,
      CAN_RGB = 5,
      LIGHT_LEVEL = 6,
      COLOR_TEMP = 7,
      HUE = 8,
      SATURATION = 9,
      ROOM_VIEW_POSITION = 10
    };

    NSPanelLightStatus& operator=(const NSPanelLightStatus& rhs)
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_can_dim(rhs.get_can_dim());
      set_can_color_temperature(rhs.get_can_color_temperature());
      set_can_rgb(rhs.get_can_rgb());
      set_light_level(rhs.get_light_level());
      set_color_temp(rhs.get_color_temp());
      set_hue(rhs.get_hue());
      set_saturation(rhs.get_saturation());
      set_room_view_position(rhs.get_room_view_position());
      return *this;
    }

    NSPanelLightStatus& operator=(const NSPanelLightStatus&& rhs) noexcept
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_can_dim(rhs.get_can_dim());
      set_can_color_temperature(rhs.get_can_color_temperature());
      set_can_rgb(rhs.get_can_rgb());
      set_light_level(rhs.get_light_level());
      set_color_temp(rhs.get_color_temp());
      set_hue(rhs.get_hue());
      set_saturation(rhs.get_saturation());
      set_room_view_position(rhs.get_room_view_position());
      return *this;
    }

    static constexpr char const* ID_NAME = "id";
    inline void clear_id() { id_.clear(); }
    inline void set_id(const int32_t& value) { id_ = value; }
    inline void set_id(const int32_t&& value) { id_ = value; }
    inline int32_t& mutable_id() { return id_.get(); }
    inline const int32_t& get_id() const { return id_.get(); }
    inline int32_t id() const { return id_.get(); }

    static constexpr char const* NAME_NAME = "name";
    inline void clear_name() { name_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelLightStatus_name_LENGTH>& mutable_name() { return name_; }
    inline void set_name(const ::EmbeddedProto::FieldString<NSPanelLightStatus_name_LENGTH>& rhs) { name_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelLightStatus_name_LENGTH>& get_name() const { return name_; }
    inline const char* name() const { return name_.get_const(); }

    static constexpr char const* CAN_DIM_NAME = "can_dim";
    inline void clear_can_dim() { can_dim_.clear(); }
    inline void set_can_dim(const bool& value) { can_dim_ = value; }
    inline void set_can_dim(const bool&& value) { can_dim_ = value; }
    inline bool& mutable_can_dim() { return can_dim_.get(); }
    inline const bool& get_can_dim() const { return can_dim_.get(); }
    inline bool can_dim() const { return can_dim_.get(); }

    static constexpr char const* CAN_COLOR_TEMPERATURE_NAME = "can_color_temperature";
    inline void clear_can_color_temperature() { can_color_temperature_.clear(); }
    inline void set_can_color_temperature(const bool& value) { can_color_temperature_ = value; }
    inline void set_can_color_temperature(const bool&& value) { can_color_temperature_ = value; }
    inline bool& mutable_can_color_temperature() { return can_color_temperature_.get(); }
    inline const bool& get_can_color_temperature() const { return can_color_temperature_.get(); }
    inline bool can_color_temperature() const { return can_color_temperature_.get(); }

    static constexpr char const* CAN_RGB_NAME = "can_rgb";
    inline void clear_can_rgb() { can_rgb_.clear(); }
    inline void set_can_rgb(const bool& value) { can_rgb_ = value; }
    inline void set_can_rgb(const bool&& value) { can_rgb_ = value; }
    inline bool& mutable_can_rgb() { return can_rgb_.get(); }
    inline const bool& get_can_rgb() const { return can_rgb_.get(); }
    inline bool can_rgb() const { return can_rgb_.get(); }

    static constexpr char const* LIGHT_LEVEL_NAME = "light_level";
    inline void clear_light_level() { light_level_.clear(); }
    inline void set_light_level(const int32_t& value) { light_level_ = value; }
    inline void set_light_level(const int32_t&& value) { light_level_ = value; }
    inline int32_t& mutable_light_level() { return light_level_.get(); }
    inline const int32_t& get_light_level() const { return light_level_.get(); }
    inline int32_t light_level() const { return light_level_.get(); }

    static constexpr char const* COLOR_TEMP_NAME = "color_temp";
    inline void clear_color_temp() { color_temp_.clear(); }
    inline void set_color_temp(const int32_t& value) { color_temp_ = value; }
    inline void set_color_temp(const int32_t&& value) { color_temp_ = value; }
    inline int32_t& mutable_color_temp() { return color_temp_.get(); }
    inline const int32_t& get_color_temp() const { return color_temp_.get(); }
    inline int32_t color_temp() const { return color_temp_.get(); }

    static constexpr char const* HUE_NAME = "hue";
    inline void clear_hue() { hue_.clear(); }
    inline void set_hue(const int32_t& value) { hue_ = value; }
    inline void set_hue(const int32_t&& value) { hue_ = value; }
    inline int32_t& mutable_hue() { return hue_.get(); }
    inline const int32_t& get_hue() const { return hue_.get(); }
    inline int32_t hue() const { return hue_.get(); }

    static constexpr char const* SATURATION_NAME = "saturation";
    inline void clear_saturation() { saturation_.clear(); }
    inline void set_saturation(const int32_t& value) { saturation_ = value; }
    inline void set_saturation(const int32_t&& value) { saturation_ = value; }
    inline int32_t& mutable_saturation() { return saturation_.get(); }
    inline const int32_t& get_saturation() const { return saturation_.get(); }
    inline int32_t saturation() const { return saturation_.get(); }

    static constexpr char const* ROOM_VIEW_POSITION_NAME = "room_view_position";
    inline void clear_room_view_position() { room_view_position_.clear(); }
    inline void set_room_view_position(const int32_t& value) { room_view_position_ = value; }
    inline void set_room_view_position(const int32_t&& value) { room_view_position_ = value; }
    inline int32_t& mutable_room_view_position() { return room_view_position_.get(); }
    inline const int32_t& get_room_view_position() const { return room_view_position_.get(); }
    inline int32_t room_view_position() const { return room_view_position_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((0 != id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ID), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = name_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAME), buffer, false);
      }

      if((false != can_dim_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = can_dim_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CAN_DIM), buffer, false);
      }

      if((false != can_color_temperature_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = can_color_temperature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CAN_COLOR_TEMPERATURE), buffer, false);
      }

      if((false != can_rgb_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = can_rgb_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CAN_RGB), buffer, false);
      }

      if((0 != light_level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = light_level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::LIGHT_LEVEL), buffer, false);
      }

      if((0 != color_temp_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = color_temp_.serialize_with_id(static_cast<uint32_t>(FieldNumber::COLOR_TEMP), buffer, false);
      }

      if((0 != hue_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = hue_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HUE), buffer, false);
      }

      if((0 != saturation_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = saturation_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SATURATION), buffer, false);
      }

      if((0 != room_view_position_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = room_view_position_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ROOM_VIEW_POSITION), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::ID:
            return_value = id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NAME:
            return_value = name_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CAN_DIM:
            return_value = can_dim_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CAN_COLOR_TEMPERATURE:
            return_value = can_color_temperature_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CAN_RGB:
            return_value = can_rgb_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::LIGHT_LEVEL:
            return_value = light_level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::COLOR_TEMP:
            return_value = color_temp_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::HUE:
            return_value = hue_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SATURATION:
            return_value = saturation_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::ROOM_VIEW_POSITION:
            return_value = room_view_position_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_id();
      clear_name();
      clear_can_dim();
      clear_can_color_temperature();
      clear_can_rgb();
      clear_light_level();
      clear_color_temp();
      clear_hue();
      clear_saturation();
      clear_room_view_position();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::ID:
          name = ID_NAME;
          break;
        case FieldNumber::NAME:
          name = NAME_NAME;
          break;
        case FieldNumber::CAN_DIM:
          name = CAN_DIM_NAME;
          break;
        case FieldNumber::CAN_COLOR_TEMPERATURE:
          name = CAN_COLOR_TEMPERATURE_NAME;
          break;
        case FieldNumber::CAN_RGB:
          name = CAN_RGB_NAME;
          break;
        case FieldNumber::LIGHT_LEVEL:
          name = LIGHT_LEVEL_NAME;
          break;
        case FieldNumber::COLOR_TEMP:
          name = COLOR_TEMP_NAME;
          break;
        case FieldNumber::HUE:
          name = HUE_NAME;
          break;
        case FieldNumber::SATURATION:
          name = SATURATION_NAME;
          break;
        case FieldNumber::ROOM_VIEW_POSITION:
          name = ROOM_VIEW_POSITION_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = id_.to_string(left_chars, indent_level + 2, ID_NAME, true);
      left_chars = name_.to_string(left_chars, indent_level + 2, NAME_NAME, false);
      left_chars = can_dim_.to_string(left_chars, indent_level + 2, CAN_DIM_NAME, false);
      left_chars = can_color_temperature_.to_string(left_chars, indent_level + 2, CAN_COLOR_TEMPERATURE_NAME, false);
      left_chars = can_rgb_.to_string(left_chars, indent_level + 2, CAN_RGB_NAME, false);
      left_chars = light_level_.to_string(left_chars, indent_level + 2, LIGHT_LEVEL_NAME, false);
      left_chars = color_temp_.to_string(left_chars, indent_level + 2, COLOR_TEMP_NAME, false);
      left_chars = hue_.to_string(left_chars, indent_level + 2, HUE_NAME, false);
      left_chars = saturation_.to_string(left_chars, indent_level + 2, SATURATION_NAME, false);
      left_chars = room_view_position_.to_string(left_chars, indent_level + 2, ROOM_VIEW_POSITION_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::int32 id_ = 0;
      ::EmbeddedProto::FieldString<NSPanelLightStatus_name_LENGTH> name_;
      EmbeddedProto::boolean can_dim_ = false;
      EmbeddedProto::boolean can_color_temperature_ = false;
      EmbeddedProto::boolean can_rgb_ = false;
      EmbeddedProto::int32 light_level_ = 0;
      EmbeddedProto::int32 color_temp_ = 0;
      EmbeddedProto::int32 hue_ = 0;
      EmbeddedProto::int32 saturation_ = 0;
      EmbeddedProto::int32 room_view_position_ = 0;

};

template<
    uint32_t NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH
>
class NSPanelMQTTManagerCommand final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelMQTTManagerCommand() = default;
    NSPanelMQTTManagerCommand(const NSPanelMQTTManagerCommand& rhs )
    {
      if(rhs.get_which_CommandData() != which_CommandData_)
      {
        // First delete the old object in the oneof.
        clear_CommandData();
      }

      switch(rhs.get_which_CommandData())
      {
        case FieldNumber::FIRST_PAGE_TURN_ON:
          set_first_page_turn_on(rhs.get_first_page_turn_on());
          break;

        case FieldNumber::FIRST_PAGE_TURN_OFF:
          set_first_page_turn_off(rhs.get_first_page_turn_off());
          break;

        case FieldNumber::LIGHT_COMMAND:
          set_light_command(rhs.get_light_command());
          break;

        default:
          break;
      }

    }

    NSPanelMQTTManagerCommand(const NSPanelMQTTManagerCommand&& rhs ) noexcept
    {
      if(rhs.get_which_CommandData() != which_CommandData_)
      {
        // First delete the old object in the oneof.
        clear_CommandData();
      }

      switch(rhs.get_which_CommandData())
      {
        case FieldNumber::FIRST_PAGE_TURN_ON:
          set_first_page_turn_on(rhs.get_first_page_turn_on());
          break;

        case FieldNumber::FIRST_PAGE_TURN_OFF:
          set_first_page_turn_off(rhs.get_first_page_turn_off());
          break;

        case FieldNumber::LIGHT_COMMAND:
          set_light_command(rhs.get_light_command());
          break;

        default:
          break;
      }

    }

    ~NSPanelMQTTManagerCommand() override = default;

    enum class AffectLightsOptions : uint32_t
    {
      ALL = 0,
      TABLE_LIGHTS = 1,
      CEILING_LIGHTS = 2
    };

    template<
        uint32_t LightCommand_light_ids_REP_LENGTH
    >
    class LightCommand final: public ::EmbeddedProto::MessageInterface
    {
      public:
        LightCommand() = default;
        LightCommand(const LightCommand& rhs )
        {
          set_light_ids(rhs.get_light_ids());
          set_has_brightness(rhs.get_has_brightness());
          set_brightness(rhs.get_brightness());
          set_has_color_temperature(rhs.get_has_color_temperature());
          set_color_temperature(rhs.get_color_temperature());
          set_has_hue(rhs.get_has_hue());
          set_hue(rhs.get_hue());
          set_has_saturation(rhs.get_has_saturation());
          set_saturation(rhs.get_saturation());
        }

        LightCommand(const LightCommand&& rhs ) noexcept
        {
          set_light_ids(rhs.get_light_ids());
          set_has_brightness(rhs.get_has_brightness());
          set_brightness(rhs.get_brightness());
          set_has_color_temperature(rhs.get_has_color_temperature());
          set_color_temperature(rhs.get_color_temperature());
          set_has_hue(rhs.get_has_hue());
          set_hue(rhs.get_hue());
          set_has_saturation(rhs.get_has_saturation());
          set_saturation(rhs.get_saturation());
        }

        ~LightCommand() override = default;

        enum class FieldNumber : uint32_t
        {
          NOT_SET = 0,
          LIGHT_IDS = 1,
          HAS_BRIGHTNESS = 2,
          BRIGHTNESS = 3,
          HAS_COLOR_TEMPERATURE = 4,
          COLOR_TEMPERATURE = 5,
          HAS_HUE = 6,
          HUE = 7,
          HAS_SATURATION = 8,
          SATURATION = 9
        };

        LightCommand& operator=(const LightCommand& rhs)
        {
          set_light_ids(rhs.get_light_ids());
          set_has_brightness(rhs.get_has_brightness());
          set_brightness(rhs.get_brightness());
          set_has_color_temperature(rhs.get_has_color_temperature());
          set_color_temperature(rhs.get_color_temperature());
          set_has_hue(rhs.get_has_hue());
          set_hue(rhs.get_hue());
          set_has_saturation(rhs.get_has_saturation());
          set_saturation(rhs.get_saturation());
          return *this;
        }

        LightCommand& operator=(const LightCommand&& rhs) noexcept
        {
          set_light_ids(rhs.get_light_ids());
          set_has_brightness(rhs.get_has_brightness());
          set_brightness(rhs.get_brightness());
          set_has_color_temperature(rhs.get_has_color_temperature());
          set_color_temperature(rhs.get_color_temperature());
          set_has_hue(rhs.get_has_hue());
          set_hue(rhs.get_hue());
          set_has_saturation(rhs.get_has_saturation());
          set_saturation(rhs.get_saturation());
          return *this;
        }

        static constexpr char const* LIGHT_IDS_NAME = "light_ids";
        inline const EmbeddedProto::int32& light_ids(uint32_t index) const { return light_ids_[index]; }
        inline void clear_light_ids() { light_ids_.clear(); }
        inline void set_light_ids(uint32_t index, const EmbeddedProto::int32& value) { light_ids_.set(index, value); }
        inline void set_light_ids(uint32_t index, const EmbeddedProto::int32&& value) { light_ids_.set(index, value); }
        inline void set_light_ids(const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, LightCommand_light_ids_REP_LENGTH>& values) { light_ids_ = values; }
        inline void add_light_ids(const EmbeddedProto::int32& value) { light_ids_.add(value); }
        inline ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, LightCommand_light_ids_REP_LENGTH>& mutable_light_ids() { return light_ids_; }
        inline EmbeddedProto::int32& mutable_light_ids(uint32_t index) { return light_ids_[index]; }
        inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, LightCommand_light_ids_REP_LENGTH>& get_light_ids() const { return light_ids_; }
        inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, LightCommand_light_ids_REP_LENGTH>& light_ids() const { return light_ids_; }

        static constexpr char const* HAS_BRIGHTNESS_NAME = "has_brightness";
        inline void clear_has_brightness() { has_brightness_.clear(); }
        inline void set_has_brightness(const bool& value) { has_brightness_ = value; }
        inline void set_has_brightness(const bool&& value) { has_brightness_ = value; }
        inline bool& mutable_has_brightness() { return has_brightness_.get(); }
        inline const bool& get_has_brightness() const { return has_brightness_.get(); }
        inline bool has_brightness() const { return has_brightness_.get(); }

        static constexpr char const* BRIGHTNESS_NAME = "brightness";
        inline void clear_brightness() { brightness_.clear(); }
        inline void set_brightness(const int32_t& value) { brightness_ = value; }
        inline void set_brightness(const int32_t&& value) { brightness_ = value; }
        inline int32_t& mutable_brightness() { return brightness_.get(); }
        inline const int32_t& get_brightness() const { return brightness_.get(); }
        inline int32_t brightness() const { return brightness_.get(); }

        static constexpr char const* HAS_COLOR_TEMPERATURE_NAME = "has_color_temperature";
        inline void clear_has_color_temperature() { has_color_temperature_.clear(); }
        inline void set_has_color_temperature(const bool& value) { has_color_temperature_ = value; }
        inline void set_has_color_temperature(const bool&& value) { has_color_temperature_ = value; }
        inline bool& mutable_has_color_temperature() { return has_color_temperature_.get(); }
        inline const bool& get_has_color_temperature() const { return has_color_temperature_.get(); }
        inline bool has_color_temperature() const { return has_color_temperature_.get(); }

        static constexpr char const* COLOR_TEMPERATURE_NAME = "color_temperature";
        inline void clear_color_temperature() { color_temperature_.clear(); }
        inline void set_color_temperature(const int32_t& value) { color_temperature_ = value; }
        inline void set_color_temperature(const int32_t&& value) { color_temperature_ = value; }
        inline int32_t& mutable_color_temperature() { return color_temperature_.get(); }
        inline const int32_t& get_color_temperature() const { return color_temperature_.get(); }
        inline int32_t color_temperature() const { return color_temperature_.get(); }

        static constexpr char const* HAS_HUE_NAME = "has_hue";
        inline void clear_has_hue() { has_hue_.clear(); }
        inline void set_has_hue(const bool& value) { has_hue_ = value; }
        inline void set_has_hue(const bool&& value) { has_hue_ = value; }
        inline bool& mutable_has_hue() { return has_hue_.get(); }
        inline const bool& get_has_hue() const { return has_hue_.get(); }
        inline bool has_hue() const { return has_hue_.get(); }

        static constexpr char const* HUE_NAME = "hue";
        inline void clear_hue() { hue_.clear(); }
        inline void set_hue(const int32_t& value) { hue_ = value; }
        inline void set_hue(const int32_t&& value) { hue_ = value; }
        inline int32_t& mutable_hue() { return hue_.get(); }
        inline const int32_t& get_hue() const { return hue_.get(); }
        inline int32_t hue() const { return hue_.get(); }

        static constexpr char const* HAS_SATURATION_NAME = "has_saturation";
        inline void clear_has_saturation() { has_saturation_.clear(); }
        inline void set_has_saturation(const bool& value) { has_saturation_ = value; }
        inline void set_has_saturation(const bool&& value) { has_saturation_ = value; }
        inline bool& mutable_has_saturation() { return has_saturation_.get(); }
        inline const bool& get_has_saturation() const { return has_saturation_.get(); }
        inline bool has_saturation() const { return has_saturation_.get(); }

        static constexpr char const* SATURATION_NAME = "saturation";
        inline void clear_saturation() { saturation_.clear(); }
        inline void set_saturation(const int32_t& value) { saturation_ = value; }
        inline void set_saturation(const int32_t&& value) { saturation_ = value; }
        inline int32_t& mutable_saturation() { return saturation_.get(); }
        inline const int32_t& get_saturation() const { return saturation_.get(); }
        inline int32_t saturation() const { return saturation_.get(); }


        ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = light_ids_.serialize_with_id(static_cast<uint32_t>(FieldNumber::LIGHT_IDS), buffer, false);
          }

          if((false != has_brightness_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = has_brightness_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_BRIGHTNESS), buffer, false);
          }

          if((0 != brightness_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = brightness_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BRIGHTNESS), buffer, false);
          }

          if((false != has_color_temperature_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = has_color_temperature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_COLOR_TEMPERATURE), buffer, false);
          }

          if((0 != color_temperature_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = color_temperature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::COLOR_TEMPERATURE), buffer, false);
          }

          if((false != has_hue_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = has_hue_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_HUE), buffer, false);
          }

          if((0 != hue_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = hue_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HUE), buffer, false);
          }

          if((false != has_saturation_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = has_saturation_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_SATURATION), buffer, false);
          }

          if((0 != saturation_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = saturation_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SATURATION), buffer, false);
          }

          return return_value;
        };

        ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
          ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
          uint32_t id_number = 0;
          FieldNumber id_tag = FieldNumber::NOT_SET;

          ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
          while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
          {
            id_tag = static_cast<FieldNumber>(id_number);
            switch(id_tag)
            {
              case FieldNumber::LIGHT_IDS:
                return_value = light_ids_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HAS_BRIGHTNESS:
                return_value = has_brightness_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::BRIGHTNESS:
                return_value = brightness_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HAS_COLOR_TEMPERATURE:
                return_value = has_color_temperature_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::COLOR_TEMPERATURE:
                return_value = color_temperature_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HAS_HUE:
                return_value = has_hue_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HUE:
                return_value = hue_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HAS_SATURATION:
                return_value = has_saturation_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::SATURATION:
                return_value = saturation_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::NOT_SET:
                return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
                break;

              default:
                return_value = skip_unknown_field(buffer, wire_type);
                break;
            }

            if(::EmbeddedProto::Error::NO_ERRORS == return_value)
            {
              // Read the next tag.
              tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
            }
          }

          // When an error was detect while reading the tag but no other errors where found, set it in the return value.
          if((::EmbeddedProto::Error::NO_ERRORS == return_value)
             && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
             && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
          {
            return_value = tag_value;
          }

          return return_value;
        };

        void clear() override
        {
          clear_light_ids();
          clear_has_brightness();
          clear_brightness();
          clear_has_color_temperature();
          clear_color_temperature();
          clear_has_hue();
          clear_hue();
          clear_has_saturation();
          clear_saturation();

        }

        static char const* field_number_to_name(const FieldNumber fieldNumber)
        {
          char const* name = nullptr;
          switch(fieldNumber)
          {
            case FieldNumber::LIGHT_IDS:
              name = LIGHT_IDS_NAME;
              break;
            case FieldNumber::HAS_BRIGHTNESS:
              name = HAS_BRIGHTNESS_NAME;
              break;
            case FieldNumber::BRIGHTNESS:
              name = BRIGHTNESS_NAME;
              break;
            case FieldNumber::HAS_COLOR_TEMPERATURE:
              name = HAS_COLOR_TEMPERATURE_NAME;
              break;
            case FieldNumber::COLOR_TEMPERATURE:
              name = COLOR_TEMPERATURE_NAME;
              break;
            case FieldNumber::HAS_HUE:
              name = HAS_HUE_NAME;
              break;
            case FieldNumber::HUE:
              name = HUE_NAME;
              break;
            case FieldNumber::HAS_SATURATION:
              name = HAS_SATURATION_NAME;
              break;
            case FieldNumber::SATURATION:
              name = SATURATION_NAME;
              break;
            default:
              name = "Invalid FieldNumber";
              break;
          }
          return name;
        }

    #ifdef MSG_TO_STRING

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
        {
          return this->to_string(str, 0, nullptr, true);
        }

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
        {
          ::EmbeddedProto::string_view left_chars = str;
          int32_t n_chars_used = 0;

          if(!first_field)
          {
            // Add a comma behind the previous field.
            n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
            if(0 < n_chars_used)
            {
              // Update the character pointer and characters left in the array.
              left_chars.data += n_chars_used;
              left_chars.size -= n_chars_used;
            }
          }

          if(nullptr != name)
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
            }
          }
          else
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
            }
          }
          
          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          left_chars = light_ids_.to_string(left_chars, indent_level + 2, LIGHT_IDS_NAME, true);
          left_chars = has_brightness_.to_string(left_chars, indent_level + 2, HAS_BRIGHTNESS_NAME, false);
          left_chars = brightness_.to_string(left_chars, indent_level + 2, BRIGHTNESS_NAME, false);
          left_chars = has_color_temperature_.to_string(left_chars, indent_level + 2, HAS_COLOR_TEMPERATURE_NAME, false);
          left_chars = color_temperature_.to_string(left_chars, indent_level + 2, COLOR_TEMPERATURE_NAME, false);
          left_chars = has_hue_.to_string(left_chars, indent_level + 2, HAS_HUE_NAME, false);
          left_chars = hue_.to_string(left_chars, indent_level + 2, HUE_NAME, false);
          left_chars = has_saturation_.to_string(left_chars, indent_level + 2, HAS_SATURATION_NAME, false);
          left_chars = saturation_.to_string(left_chars, indent_level + 2, SATURATION_NAME, false);
      
          if( 0 == indent_level) 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
          }
          else 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
          }

          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          return left_chars;
        }

    #endif // End of MSG_TO_STRING

      private:


          ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, LightCommand_light_ids_REP_LENGTH> light_ids_;
          EmbeddedProto::boolean has_brightness_ = false;
          EmbeddedProto::int32 brightness_ = 0;
          EmbeddedProto::boolean has_color_temperature_ = false;
          EmbeddedProto::int32 color_temperature_ = 0;
          EmbeddedProto::boolean has_hue_ = false;
          EmbeddedProto::int32 hue_ = 0;
          EmbeddedProto::boolean has_saturation_ = false;
          EmbeddedProto::int32 saturation_ = 0;

    };

    class FirstPageTurnLightOff final: public ::EmbeddedProto::MessageInterface
    {
      public:
        FirstPageTurnLightOff() = default;
        FirstPageTurnLightOff(const FirstPageTurnLightOff& rhs )
        {
          set_affect_lights(rhs.get_affect_lights());
          set_global(rhs.get_global());
        }

        FirstPageTurnLightOff(const FirstPageTurnLightOff&& rhs ) noexcept
        {
          set_affect_lights(rhs.get_affect_lights());
          set_global(rhs.get_global());
        }

        ~FirstPageTurnLightOff() override = default;

        enum class FieldNumber : uint32_t
        {
          NOT_SET = 0,
          AFFECT_LIGHTS = 1,
          GLOBAL = 2
        };

        FirstPageTurnLightOff& operator=(const FirstPageTurnLightOff& rhs)
        {
          set_affect_lights(rhs.get_affect_lights());
          set_global(rhs.get_global());
          return *this;
        }

        FirstPageTurnLightOff& operator=(const FirstPageTurnLightOff&& rhs) noexcept
        {
          set_affect_lights(rhs.get_affect_lights());
          set_global(rhs.get_global());
          return *this;
        }

        static constexpr char const* AFFECT_LIGHTS_NAME = "affect_lights";
        inline void clear_affect_lights() { affect_lights_.clear(); }
        inline void set_affect_lights(const AffectLightsOptions& value) { affect_lights_ = value; }
        inline void set_affect_lights(const AffectLightsOptions&& value) { affect_lights_ = value; }
        inline const AffectLightsOptions& get_affect_lights() const { return affect_lights_.get(); }
        inline AffectLightsOptions affect_lights() const { return affect_lights_.get(); }

        static constexpr char const* GLOBAL_NAME = "global";
        inline void clear_global() { global_.clear(); }
        inline void set_global(const bool& value) { global_ = value; }
        inline void set_global(const bool&& value) { global_ = value; }
        inline bool& mutable_global() { return global_.get(); }
        inline const bool& get_global() const { return global_.get(); }
        inline bool global() const { return global_.get(); }


        ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

          if((static_cast<AffectLightsOptions>(0) != affect_lights_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = affect_lights_.serialize_with_id(static_cast<uint32_t>(FieldNumber::AFFECT_LIGHTS), buffer, false);
          }

          if((false != global_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = global_.serialize_with_id(static_cast<uint32_t>(FieldNumber::GLOBAL), buffer, false);
          }

          return return_value;
        };

        ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
          ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
          uint32_t id_number = 0;
          FieldNumber id_tag = FieldNumber::NOT_SET;

          ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
          while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
          {
            id_tag = static_cast<FieldNumber>(id_number);
            switch(id_tag)
            {
              case FieldNumber::AFFECT_LIGHTS:
                return_value = affect_lights_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::GLOBAL:
                return_value = global_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::NOT_SET:
                return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
                break;

              default:
                return_value = skip_unknown_field(buffer, wire_type);
                break;
            }

            if(::EmbeddedProto::Error::NO_ERRORS == return_value)
            {
              // Read the next tag.
              tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
            }
          }

          // When an error was detect while reading the tag but no other errors where found, set it in the return value.
          if((::EmbeddedProto::Error::NO_ERRORS == return_value)
             && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
             && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
          {
            return_value = tag_value;
          }

          return return_value;
        };

        void clear() override
        {
          clear_affect_lights();
          clear_global();

        }

        static char const* field_number_to_name(const FieldNumber fieldNumber)
        {
          char const* name = nullptr;
          switch(fieldNumber)
          {
            case FieldNumber::AFFECT_LIGHTS:
              name = AFFECT_LIGHTS_NAME;
              break;
            case FieldNumber::GLOBAL:
              name = GLOBAL_NAME;
              break;
            default:
              name = "Invalid FieldNumber";
              break;
          }
          return name;
        }

    #ifdef MSG_TO_STRING

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
        {
          return this->to_string(str, 0, nullptr, true);
        }

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
        {
          ::EmbeddedProto::string_view left_chars = str;
          int32_t n_chars_used = 0;

          if(!first_field)
          {
            // Add a comma behind the previous field.
            n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
            if(0 < n_chars_used)
            {
              // Update the character pointer and characters left in the array.
              left_chars.data += n_chars_used;
              left_chars.size -= n_chars_used;
            }
          }

          if(nullptr != name)
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
            }
          }
          else
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
            }
          }
          
          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          left_chars = affect_lights_.to_string(left_chars, indent_level + 2, AFFECT_LIGHTS_NAME, true);
          left_chars = global_.to_string(left_chars, indent_level + 2, GLOBAL_NAME, false);
      
          if( 0 == indent_level) 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
          }
          else 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
          }

          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          return left_chars;
        }

    #endif // End of MSG_TO_STRING

      private:


          EmbeddedProto::enumeration<AffectLightsOptions> affect_lights_ = static_cast<AffectLightsOptions>(0);
          EmbeddedProto::boolean global_ = false;

    };

    class FirstPageTurnLightOn final: public ::EmbeddedProto::MessageInterface
    {
      public:
        FirstPageTurnLightOn() = default;
        FirstPageTurnLightOn(const FirstPageTurnLightOn& rhs )
        {
          set_affect_lights(rhs.get_affect_lights());
          set_brightness_slider_value(rhs.get_brightness_slider_value());
          set_kelvin_slider_value(rhs.get_kelvin_slider_value());
          set_selected_room(rhs.get_selected_room());
          set_global(rhs.get_global());
          set_has_brightness_value(rhs.get_has_brightness_value());
          set_has_kelvin_value(rhs.get_has_kelvin_value());
        }

        FirstPageTurnLightOn(const FirstPageTurnLightOn&& rhs ) noexcept
        {
          set_affect_lights(rhs.get_affect_lights());
          set_brightness_slider_value(rhs.get_brightness_slider_value());
          set_kelvin_slider_value(rhs.get_kelvin_slider_value());
          set_selected_room(rhs.get_selected_room());
          set_global(rhs.get_global());
          set_has_brightness_value(rhs.get_has_brightness_value());
          set_has_kelvin_value(rhs.get_has_kelvin_value());
        }

        ~FirstPageTurnLightOn() override = default;

        enum class FieldNumber : uint32_t
        {
          NOT_SET = 0,
          AFFECT_LIGHTS = 1,
          BRIGHTNESS_SLIDER_VALUE = 2,
          KELVIN_SLIDER_VALUE = 3,
          SELECTED_ROOM = 4,
          GLOBAL = 5,
          HAS_BRIGHTNESS_VALUE = 6,
          HAS_KELVIN_VALUE = 7
        };

        FirstPageTurnLightOn& operator=(const FirstPageTurnLightOn& rhs)
        {
          set_affect_lights(rhs.get_affect_lights());
          set_brightness_slider_value(rhs.get_brightness_slider_value());
          set_kelvin_slider_value(rhs.get_kelvin_slider_value());
          set_selected_room(rhs.get_selected_room());
          set_global(rhs.get_global());
          set_has_brightness_value(rhs.get_has_brightness_value());
          set_has_kelvin_value(rhs.get_has_kelvin_value());
          return *this;
        }

        FirstPageTurnLightOn& operator=(const FirstPageTurnLightOn&& rhs) noexcept
        {
          set_affect_lights(rhs.get_affect_lights());
          set_brightness_slider_value(rhs.get_brightness_slider_value());
          set_kelvin_slider_value(rhs.get_kelvin_slider_value());
          set_selected_room(rhs.get_selected_room());
          set_global(rhs.get_global());
          set_has_brightness_value(rhs.get_has_brightness_value());
          set_has_kelvin_value(rhs.get_has_kelvin_value());
          return *this;
        }

        static constexpr char const* AFFECT_LIGHTS_NAME = "affect_lights";
        inline void clear_affect_lights() { affect_lights_.clear(); }
        inline void set_affect_lights(const AffectLightsOptions& value) { affect_lights_ = value; }
        inline void set_affect_lights(const AffectLightsOptions&& value) { affect_lights_ = value; }
        inline const AffectLightsOptions& get_affect_lights() const { return affect_lights_.get(); }
        inline AffectLightsOptions affect_lights() const { return affect_lights_.get(); }

        static constexpr char const* BRIGHTNESS_SLIDER_VALUE_NAME = "brightness_slider_value";
        inline void clear_brightness_slider_value() { brightness_slider_value_.clear(); }
        inline void set_brightness_slider_value(const int32_t& value) { brightness_slider_value_ = value; }
        inline void set_brightness_slider_value(const int32_t&& value) { brightness_slider_value_ = value; }
        inline int32_t& mutable_brightness_slider_value() { return brightness_slider_value_.get(); }
        inline const int32_t& get_brightness_slider_value() const { return brightness_slider_value_.get(); }
        inline int32_t brightness_slider_value() const { return brightness_slider_value_.get(); }

        static constexpr char const* KELVIN_SLIDER_VALUE_NAME = "kelvin_slider_value";
        inline void clear_kelvin_slider_value() { kelvin_slider_value_.clear(); }
        inline void set_kelvin_slider_value(const int32_t& value) { kelvin_slider_value_ = value; }
        inline void set_kelvin_slider_value(const int32_t&& value) { kelvin_slider_value_ = value; }
        inline int32_t& mutable_kelvin_slider_value() { return kelvin_slider_value_.get(); }
        inline const int32_t& get_kelvin_slider_value() const { return kelvin_slider_value_.get(); }
        inline int32_t kelvin_slider_value() const { return kelvin_slider_value_.get(); }

        static constexpr char const* SELECTED_ROOM_NAME = "selected_room";
        inline void clear_selected_room() { selected_room_.clear(); }
        inline void set_selected_room(const int32_t& value) { selected_room_ = value; }
        inline void set_selected_room(const int32_t&& value) { selected_room_ = value; }
        inline int32_t& mutable_selected_room() { return selected_room_.get(); }
        inline const int32_t& get_selected_room() const { return selected_room_.get(); }
        inline int32_t selected_room() const { return selected_room_.get(); }

        static constexpr char const* GLOBAL_NAME = "global";
        inline void clear_global() { global_.clear(); }
        inline void set_global(const bool& value) { global_ = value; }
        inline void set_global(const bool&& value) { global_ = value; }
        inline bool& mutable_global() { return global_.get(); }
        inline const bool& get_global() const { return global_.get(); }
        inline bool global() const { return global_.get(); }

        static constexpr char const* HAS_BRIGHTNESS_VALUE_NAME = "has_brightness_value";
        inline void clear_has_brightness_value() { has_brightness_value_.clear(); }
        inline void set_has_brightness_value(const bool& value) { has_brightness_value_ = value; }
        inline void set_has_brightness_value(const bool&& value) { has_brightness_value_ = value; }
        inline bool& mutable_has_brightness_value() { return has_brightness_value_.get(); }
        inline const bool& get_has_brightness_value() const { return has_brightness_value_.get(); }
        inline bool has_brightness_value() const { return has_brightness_value_.get(); }

        static constexpr char const* HAS_KELVIN_VALUE_NAME = "has_kelvin_value";
        inline void clear_has_kelvin_value() { has_kelvin_value_.clear(); }
        inline void set_has_kelvin_value(const bool& value) { has_kelvin_value_ = value; }
        inline void set_has_kelvin_value(const bool&& value) { has_kelvin_value_ = value; }
        inline bool& mutable_has_kelvin_value() { return has_kelvin_value_.get(); }
        inline const bool& get_has_kelvin_value() const { return has_kelvin_value_.get(); }
        inline bool has_kelvin_value() const { return has_kelvin_value_.get(); }


        ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

          if((static_cast<AffectLightsOptions>(0) != affect_lights_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = affect_lights_.serialize_with_id(static_cast<uint32_t>(FieldNumber::AFFECT_LIGHTS), buffer, false);
          }

          if((0 != brightness_slider_value_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = brightness_slider_value_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BRIGHTNESS_SLIDER_VALUE), buffer, false);
          }

          if((0 != kelvin_slider_value_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = kelvin_slider_value_.serialize_with_id(static_cast<uint32_t>(FieldNumber::KELVIN_SLIDER_VALUE), buffer, false);
          }

          if((0 != selected_room_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = selected_room_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SELECTED_ROOM), buffer, false);
          }

          if((false != global_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = global_.serialize_with_id(static_cast<uint32_t>(FieldNumber::GLOBAL), buffer, false);
          }

          if((false != has_brightness_value_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = has_brightness_value_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_BRIGHTNESS_VALUE), buffer, false);
          }

          if((false != has_kelvin_value_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = has_kelvin_value_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_KELVIN_VALUE), buffer, false);
          }

          return return_value;
        };

        ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
          ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
          uint32_t id_number = 0;
          FieldNumber id_tag = FieldNumber::NOT_SET;

          ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
          while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
          {
            id_tag = static_cast<FieldNumber>(id_number);
            switch(id_tag)
            {
              case FieldNumber::AFFECT_LIGHTS:
                return_value = affect_lights_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::BRIGHTNESS_SLIDER_VALUE:
                return_value = brightness_slider_value_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::KELVIN_SLIDER_VALUE:
                return_value = kelvin_slider_value_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::SELECTED_ROOM:
                return_value = selected_room_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::GLOBAL:
                return_value = global_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HAS_BRIGHTNESS_VALUE:
                return_value = has_brightness_value_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::HAS_KELVIN_VALUE:
                return_value = has_kelvin_value_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::NOT_SET:
                return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
                break;

              default:
                return_value = skip_unknown_field(buffer, wire_type);
                break;
            }

            if(::EmbeddedProto::Error::NO_ERRORS == return_value)
            {
              // Read the next tag.
              tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
            }
          }

          // When an error was detect while reading the tag but no other errors where found, set it in the return value.
          if((::EmbeddedProto::Error::NO_ERRORS == return_value)
             && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
             && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
          {
            return_value = tag_value;
          }

          return return_value;
        };

        void clear() override
        {
          clear_affect_lights();
          clear_brightness_slider_value();
          clear_kelvin_slider_value();
          clear_selected_room();
          clear_global();
          clear_has_brightness_value();
          clear_has_kelvin_value();

        }

        static char const* field_number_to_name(const FieldNumber fieldNumber)
        {
          char const* name = nullptr;
          switch(fieldNumber)
          {
            case FieldNumber::AFFECT_LIGHTS:
              name = AFFECT_LIGHTS_NAME;
              break;
            case FieldNumber::BRIGHTNESS_SLIDER_VALUE:
              name = BRIGHTNESS_SLIDER_VALUE_NAME;
              break;
            case FieldNumber::KELVIN_SLIDER_VALUE:
              name = KELVIN_SLIDER_VALUE_NAME;
              break;
            case FieldNumber::SELECTED_ROOM:
              name = SELECTED_ROOM_NAME;
              break;
            case FieldNumber::GLOBAL:
              name = GLOBAL_NAME;
              break;
            case FieldNumber::HAS_BRIGHTNESS_VALUE:
              name = HAS_BRIGHTNESS_VALUE_NAME;
              break;
            case FieldNumber::HAS_KELVIN_VALUE:
              name = HAS_KELVIN_VALUE_NAME;
              break;
            default:
              name = "Invalid FieldNumber";
              break;
          }
          return name;
        }

    #ifdef MSG_TO_STRING

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
        {
          return this->to_string(str, 0, nullptr, true);
        }

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
        {
          ::EmbeddedProto::string_view left_chars = str;
          int32_t n_chars_used = 0;

          if(!first_field)
          {
            // Add a comma behind the previous field.
            n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
            if(0 < n_chars_used)
            {
              // Update the character pointer and characters left in the array.
              left_chars.data += n_chars_used;
              left_chars.size -= n_chars_used;
            }
          }

          if(nullptr != name)
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
            }
          }
          else
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
            }
          }
          
          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          left_chars = affect_lights_.to_string(left_chars, indent_level + 2, AFFECT_LIGHTS_NAME, true);
          left_chars = brightness_slider_value_.to_string(left_chars, indent_level + 2, BRIGHTNESS_SLIDER_VALUE_NAME, false);
          left_chars = kelvin_slider_value_.to_string(left_chars, indent_level + 2, KELVIN_SLIDER_VALUE_NAME, false);
          left_chars = selected_room_.to_string(left_chars, indent_level + 2, SELECTED_ROOM_NAME, false);
          left_chars = global_.to_string(left_chars, indent_level + 2, GLOBAL_NAME, false);
          left_chars = has_brightness_value_.to_string(left_chars, indent_level + 2, HAS_BRIGHTNESS_VALUE_NAME, false);
          left_chars = has_kelvin_value_.to_string(left_chars, indent_level + 2, HAS_KELVIN_VALUE_NAME, false);
      
          if( 0 == indent_level) 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
          }
          else 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
          }

          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          return left_chars;
        }

    #endif // End of MSG_TO_STRING

      private:


          EmbeddedProto::enumeration<AffectLightsOptions> affect_lights_ = static_cast<AffectLightsOptions>(0);
          EmbeddedProto::int32 brightness_slider_value_ = 0;
          EmbeddedProto::int32 kelvin_slider_value_ = 0;
          EmbeddedProto::int32 selected_room_ = 0;
          EmbeddedProto::boolean global_ = false;
          EmbeddedProto::boolean has_brightness_value_ = false;
          EmbeddedProto::boolean has_kelvin_value_ = false;

    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      FIRST_PAGE_TURN_ON = 1,
      FIRST_PAGE_TURN_OFF = 2,
      LIGHT_COMMAND = 3
    };

    NSPanelMQTTManagerCommand& operator=(const NSPanelMQTTManagerCommand& rhs)
    {
      if(rhs.get_which_CommandData() != which_CommandData_)
      {
        // First delete the old object in the oneof.
        clear_CommandData();
      }

      switch(rhs.get_which_CommandData())
      {
        case FieldNumber::FIRST_PAGE_TURN_ON:
          set_first_page_turn_on(rhs.get_first_page_turn_on());
          break;

        case FieldNumber::FIRST_PAGE_TURN_OFF:
          set_first_page_turn_off(rhs.get_first_page_turn_off());
          break;

        case FieldNumber::LIGHT_COMMAND:
          set_light_command(rhs.get_light_command());
          break;

        default:
          break;
      }

      return *this;
    }

    NSPanelMQTTManagerCommand& operator=(const NSPanelMQTTManagerCommand&& rhs) noexcept
    {
      if(rhs.get_which_CommandData() != which_CommandData_)
      {
        // First delete the old object in the oneof.
        clear_CommandData();
      }

      switch(rhs.get_which_CommandData())
      {
        case FieldNumber::FIRST_PAGE_TURN_ON:
          set_first_page_turn_on(rhs.get_first_page_turn_on());
          break;

        case FieldNumber::FIRST_PAGE_TURN_OFF:
          set_first_page_turn_off(rhs.get_first_page_turn_off());
          break;

        case FieldNumber::LIGHT_COMMAND:
          set_light_command(rhs.get_light_command());
          break;

        default:
          break;
      }

      return *this;
    }

    FieldNumber get_which_CommandData() const { return which_CommandData_; }

    static constexpr char const* FIRST_PAGE_TURN_ON_NAME = "first_page_turn_on";
    inline bool has_first_page_turn_on() const
    {
      return FieldNumber::FIRST_PAGE_TURN_ON == which_CommandData_;
    }
    inline void clear_first_page_turn_on()
    {
      if(FieldNumber::FIRST_PAGE_TURN_ON == which_CommandData_)
      {
        which_CommandData_ = FieldNumber::NOT_SET;
        CommandData_.first_page_turn_on_.~FirstPageTurnLightOn();
      }
    }
    inline void set_first_page_turn_on(const FirstPageTurnLightOn& value)
    {
      if(FieldNumber::FIRST_PAGE_TURN_ON != which_CommandData_)
      {
        init_CommandData(FieldNumber::FIRST_PAGE_TURN_ON);
      }
      CommandData_.first_page_turn_on_ = value;
    }
    inline void set_first_page_turn_on(const FirstPageTurnLightOn&& value)
    {
      if(FieldNumber::FIRST_PAGE_TURN_ON != which_CommandData_)
      {
        init_CommandData(FieldNumber::FIRST_PAGE_TURN_ON);
      }
      CommandData_.first_page_turn_on_ = value;
    }
    inline FirstPageTurnLightOn& mutable_first_page_turn_on()
    {
      if(FieldNumber::FIRST_PAGE_TURN_ON != which_CommandData_)
      {
        init_CommandData(FieldNumber::FIRST_PAGE_TURN_ON);
      }
      return CommandData_.first_page_turn_on_;
    }
    inline const FirstPageTurnLightOn& get_first_page_turn_on() const { return CommandData_.first_page_turn_on_; }
    inline const FirstPageTurnLightOn& first_page_turn_on() const { return CommandData_.first_page_turn_on_; }

    static constexpr char const* FIRST_PAGE_TURN_OFF_NAME = "first_page_turn_off";
    inline bool has_first_page_turn_off() const
    {
      return FieldNumber::FIRST_PAGE_TURN_OFF == which_CommandData_;
    }
    inline void clear_first_page_turn_off()
    {
      if(FieldNumber::FIRST_PAGE_TURN_OFF == which_CommandData_)
      {
        which_CommandData_ = FieldNumber::NOT_SET;
        CommandData_.first_page_turn_off_.~FirstPageTurnLightOff();
      }
    }
    inline void set_first_page_turn_off(const FirstPageTurnLightOff& value)
    {
      if(FieldNumber::FIRST_PAGE_TURN_OFF != which_CommandData_)
      {
        init_CommandData(FieldNumber::FIRST_PAGE_TURN_OFF);
      }
      CommandData_.first_page_turn_off_ = value;
    }
    inline void set_first_page_turn_off(const FirstPageTurnLightOff&& value)
    {
      if(FieldNumber::FIRST_PAGE_TURN_OFF != which_CommandData_)
      {
        init_CommandData(FieldNumber::FIRST_PAGE_TURN_OFF);
      }
      CommandData_.first_page_turn_off_ = value;
    }
    inline FirstPageTurnLightOff& mutable_first_page_turn_off()
    {
      if(FieldNumber::FIRST_PAGE_TURN_OFF != which_CommandData_)
      {
        init_CommandData(FieldNumber::FIRST_PAGE_TURN_OFF);
      }
      return CommandData_.first_page_turn_off_;
    }
    inline const FirstPageTurnLightOff& get_first_page_turn_off() const { return CommandData_.first_page_turn_off_; }
    inline const FirstPageTurnLightOff& first_page_turn_off() const { return CommandData_.first_page_turn_off_; }

    static constexpr char const* LIGHT_COMMAND_NAME = "light_command";
    inline bool has_light_command() const
    {
      return FieldNumber::LIGHT_COMMAND == which_CommandData_;
    }
    inline void clear_light_command()
    {
      if(FieldNumber::LIGHT_COMMAND == which_CommandData_)
      {
        which_CommandData_ = FieldNumber::NOT_SET;
        CommandData_.light_command_.~LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>();
      }
    }
    inline void set_light_command(const LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>& value)
    {
      if(FieldNumber::LIGHT_COMMAND != which_CommandData_)
      {
        init_CommandData(FieldNumber::LIGHT_COMMAND);
      }
      CommandData_.light_command_ = value;
    }
    inline void set_light_command(const LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>&& value)
    {
      if(FieldNumber::LIGHT_COMMAND != which_CommandData_)
      {
        init_CommandData(FieldNumber::LIGHT_COMMAND);
      }
      CommandData_.light_command_ = value;
    }
    inline LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>& mutable_light_command()
    {
      if(FieldNumber::LIGHT_COMMAND != which_CommandData_)
      {
        init_CommandData(FieldNumber::LIGHT_COMMAND);
      }
      return CommandData_.light_command_;
    }
    inline const LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>& get_light_command() const { return CommandData_.light_command_; }
    inline const LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>& light_command() const { return CommandData_.light_command_; }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      switch(which_CommandData_)
      {
        case FieldNumber::FIRST_PAGE_TURN_ON:
          if(has_first_page_turn_on() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = CommandData_.first_page_turn_on_.serialize_with_id(static_cast<uint32_t>(FieldNumber::FIRST_PAGE_TURN_ON), buffer, true);
          }
          break;

        case FieldNumber::FIRST_PAGE_TURN_OFF:
          if(has_first_page_turn_off() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = CommandData_.first_page_turn_off_.serialize_with_id(static_cast<uint32_t>(FieldNumber::FIRST_PAGE_TURN_OFF), buffer, true);
          }
          break;

        case FieldNumber::LIGHT_COMMAND:
          if(has_light_command() && (::EmbeddedProto::Error::NO_ERRORS == return_value))
          {
            return_value = CommandData_.light_command_.serialize_with_id(static_cast<uint32_t>(FieldNumber::LIGHT_COMMAND), buffer, true);
          }
          break;

        default:
          break;
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::FIRST_PAGE_TURN_ON:
          case FieldNumber::FIRST_PAGE_TURN_OFF:
          case FieldNumber::LIGHT_COMMAND:
            return_value = deserialize_CommandData(id_tag, buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_CommandData();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::FIRST_PAGE_TURN_ON:
          name = FIRST_PAGE_TURN_ON_NAME;
          break;
        case FieldNumber::FIRST_PAGE_TURN_OFF:
          name = FIRST_PAGE_TURN_OFF_NAME;
          break;
        case FieldNumber::LIGHT_COMMAND:
          name = LIGHT_COMMAND_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = to_string_CommandData(left_chars, indent_level + 2, true);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:



      FieldNumber which_CommandData_ = FieldNumber::NOT_SET;
      union CommandData
      {
        CommandData() {}
        ~CommandData() {}
        FirstPageTurnLightOn first_page_turn_on_;
        FirstPageTurnLightOff first_page_turn_off_;
        LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH> light_command_;
      };
      CommandData CommandData_;

      void init_CommandData(const FieldNumber field_id)
      {
        if(FieldNumber::NOT_SET != which_CommandData_)
        {
          // First delete the old object in the oneof.
          clear_CommandData();
        }

        // C++11 unions only support nontrivial members when you explicitly call the placement new statement.
        switch(field_id)
        {
          case FieldNumber::FIRST_PAGE_TURN_ON:
            new(&CommandData_.first_page_turn_on_) FirstPageTurnLightOn;
            break;

          case FieldNumber::FIRST_PAGE_TURN_OFF:
            new(&CommandData_.first_page_turn_off_) FirstPageTurnLightOff;
            break;

          case FieldNumber::LIGHT_COMMAND:
            new(&CommandData_.light_command_) LightCommand<NSPanelMQTTManagerCommand_light_command_LightCommand_light_ids_REP_LENGTH>;
            break;

          default:
            break;
         }

         which_CommandData_ = field_id;
      }

      void clear_CommandData()
      {
        switch(which_CommandData_)
        {
          case FieldNumber::FIRST_PAGE_TURN_ON:
            ::EmbeddedProto::destroy_at(&CommandData_.first_page_turn_on_);
            break;
          case FieldNumber::FIRST_PAGE_TURN_OFF:
            ::EmbeddedProto::destroy_at(&CommandData_.first_page_turn_off_);
            break;
          case FieldNumber::LIGHT_COMMAND:
            ::EmbeddedProto::destroy_at(&CommandData_.light_command_);
            break;
          default:
            break;
        }
        which_CommandData_ = FieldNumber::NOT_SET;
      }

      ::EmbeddedProto::Error deserialize_CommandData(const FieldNumber field_id, 
                                    ::EmbeddedProto::ReadBufferInterface& buffer,
                                    const ::EmbeddedProto::WireFormatter::WireType wire_type)
      {
        ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
        
        if(field_id != which_CommandData_)
        {
          init_CommandData(field_id);
        }

        switch(which_CommandData_)
        {
          case FieldNumber::FIRST_PAGE_TURN_ON:
            return_value = CommandData_.first_page_turn_on_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::FIRST_PAGE_TURN_OFF:
            return_value = CommandData_.first_page_turn_off_.deserialize_check_type(buffer, wire_type);
            break;
          case FieldNumber::LIGHT_COMMAND:
            return_value = CommandData_.light_command_.deserialize_check_type(buffer, wire_type);
            break;
          default:
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS != return_value)
        {
          clear_CommandData();
        }
        return return_value;
      }

#ifdef MSG_TO_STRING 
      ::EmbeddedProto::string_view to_string_CommandData(::EmbeddedProto::string_view& str, const uint32_t indent_level, const bool first_field) const
      {
        ::EmbeddedProto::string_view left_chars = str;

        switch(which_CommandData_)
        {
          case FieldNumber::FIRST_PAGE_TURN_ON:
            left_chars = CommandData_.first_page_turn_on_.to_string(left_chars, indent_level, FIRST_PAGE_TURN_ON_NAME, first_field);
            break;
          case FieldNumber::FIRST_PAGE_TURN_OFF:
            left_chars = CommandData_.first_page_turn_off_.to_string(left_chars, indent_level, FIRST_PAGE_TURN_OFF_NAME, first_field);
            break;
          case FieldNumber::LIGHT_COMMAND:
            left_chars = CommandData_.light_command_.to_string(left_chars, indent_level, LIGHT_COMMAND_NAME, first_field);
            break;
          default:
            break;
        }

        return left_chars;
      }

#endif // End of MSG_TO_STRING
};

template<
    uint32_t NSPanelScene_name_LENGTH
>
class NSPanelScene final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelScene() = default;
    NSPanelScene(const NSPanelScene& rhs )
    {
      set_scene_id(rhs.get_scene_id());
      set_can_save(rhs.get_can_save());
      set_name(rhs.get_name());
    }

    NSPanelScene(const NSPanelScene&& rhs ) noexcept
    {
      set_scene_id(rhs.get_scene_id());
      set_can_save(rhs.get_can_save());
      set_name(rhs.get_name());
    }

    ~NSPanelScene() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      SCENE_ID = 1,
      CAN_SAVE = 2,
      NAME = 3
    };

    NSPanelScene& operator=(const NSPanelScene& rhs)
    {
      set_scene_id(rhs.get_scene_id());
      set_can_save(rhs.get_can_save());
      set_name(rhs.get_name());
      return *this;
    }

    NSPanelScene& operator=(const NSPanelScene&& rhs) noexcept
    {
      set_scene_id(rhs.get_scene_id());
      set_can_save(rhs.get_can_save());
      set_name(rhs.get_name());
      return *this;
    }

    static constexpr char const* SCENE_ID_NAME = "scene_id";
    inline void clear_scene_id() { scene_id_.clear(); }
    inline void set_scene_id(const int32_t& value) { scene_id_ = value; }
    inline void set_scene_id(const int32_t&& value) { scene_id_ = value; }
    inline int32_t& mutable_scene_id() { return scene_id_.get(); }
    inline const int32_t& get_scene_id() const { return scene_id_.get(); }
    inline int32_t scene_id() const { return scene_id_.get(); }

    static constexpr char const* CAN_SAVE_NAME = "can_save";
    inline void clear_can_save() { can_save_.clear(); }
    inline void set_can_save(const bool& value) { can_save_ = value; }
    inline void set_can_save(const bool&& value) { can_save_ = value; }
    inline bool& mutable_can_save() { return can_save_.get(); }
    inline const bool& get_can_save() const { return can_save_.get(); }
    inline bool can_save() const { return can_save_.get(); }

    static constexpr char const* NAME_NAME = "name";
    inline void clear_name() { name_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelScene_name_LENGTH>& mutable_name() { return name_; }
    inline void set_name(const ::EmbeddedProto::FieldString<NSPanelScene_name_LENGTH>& rhs) { name_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelScene_name_LENGTH>& get_name() const { return name_; }
    inline const char* name() const { return name_.get_const(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((0 != scene_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = scene_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SCENE_ID), buffer, false);
      }

      if((false != can_save_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = can_save_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CAN_SAVE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = name_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAME), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::SCENE_ID:
            return_value = scene_id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CAN_SAVE:
            return_value = can_save_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NAME:
            return_value = name_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_scene_id();
      clear_can_save();
      clear_name();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::SCENE_ID:
          name = SCENE_ID_NAME;
          break;
        case FieldNumber::CAN_SAVE:
          name = CAN_SAVE_NAME;
          break;
        case FieldNumber::NAME:
          name = NAME_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = scene_id_.to_string(left_chars, indent_level + 2, SCENE_ID_NAME, true);
      left_chars = can_save_.to_string(left_chars, indent_level + 2, CAN_SAVE_NAME, false);
      left_chars = name_.to_string(left_chars, indent_level + 2, NAME_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::int32 scene_id_ = 0;
      EmbeddedProto::boolean can_save_ = false;
      ::EmbeddedProto::FieldString<NSPanelScene_name_LENGTH> name_;

};

template<
    uint32_t NSPanelWarning_text_LENGTH
>
class NSPanelWarning final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelWarning() = default;
    NSPanelWarning(const NSPanelWarning& rhs )
    {
      set_level(rhs.get_level());
      set_text(rhs.get_text());
    }

    NSPanelWarning(const NSPanelWarning&& rhs ) noexcept
    {
      set_level(rhs.get_level());
      set_text(rhs.get_text());
    }

    ~NSPanelWarning() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      LEVEL = 1,
      TEXT = 2
    };

    NSPanelWarning& operator=(const NSPanelWarning& rhs)
    {
      set_level(rhs.get_level());
      set_text(rhs.get_text());
      return *this;
    }

    NSPanelWarning& operator=(const NSPanelWarning&& rhs) noexcept
    {
      set_level(rhs.get_level());
      set_text(rhs.get_text());
      return *this;
    }

    static constexpr char const* LEVEL_NAME = "level";
    inline void clear_level() { level_.clear(); }
    inline void set_level(const NSPanelWarningLevel& value) { level_ = value; }
    inline void set_level(const NSPanelWarningLevel&& value) { level_ = value; }
    inline const NSPanelWarningLevel& get_level() const { return level_.get(); }
    inline NSPanelWarningLevel level() const { return level_.get(); }

    static constexpr char const* TEXT_NAME = "text";
    inline void clear_text() { text_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWarning_text_LENGTH>& mutable_text() { return text_; }
    inline void set_text(const ::EmbeddedProto::FieldString<NSPanelWarning_text_LENGTH>& rhs) { text_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWarning_text_LENGTH>& get_text() const { return text_; }
    inline const char* text() const { return text_.get_const(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((static_cast<NSPanelWarningLevel>(0) != level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::LEVEL), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = text_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TEXT), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::LEVEL:
            return_value = level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TEXT:
            return_value = text_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_level();
      clear_text();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::LEVEL:
          name = LEVEL_NAME;
          break;
        case FieldNumber::TEXT:
          name = TEXT_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = level_.to_string(left_chars, indent_level + 2, LEVEL_NAME, true);
      left_chars = text_.to_string(left_chars, indent_level + 2, TEXT_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::enumeration<NSPanelWarningLevel> level_ = static_cast<NSPanelWarningLevel>(0);
      ::EmbeddedProto::FieldString<NSPanelWarning_text_LENGTH> text_;

};

template<
    uint32_t NSPanelWeatherUpdate_forecast_items_REP_LENGTH, 
    uint32_t NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, 
    uint32_t NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_current_weather_icon_LENGTH, 
    uint32_t NSPanelWeatherUpdate_current_temperature_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_current_maxmin_temperature_LENGTH, 
    uint32_t NSPanelWeatherUpdate_current_wind_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_sunrise_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_sunset_string_LENGTH, 
    uint32_t NSPanelWeatherUpdate_current_precipitation_string_LENGTH
>
class NSPanelWeatherUpdate final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelWeatherUpdate() = default;
    NSPanelWeatherUpdate(const NSPanelWeatherUpdate& rhs )
    {
      set_forecast_items(rhs.get_forecast_items());
      set_current_weather_icon(rhs.get_current_weather_icon());
      set_current_temperature_string(rhs.get_current_temperature_string());
      set_current_maxmin_temperature(rhs.get_current_maxmin_temperature());
      set_current_wind_string(rhs.get_current_wind_string());
      set_sunrise_string(rhs.get_sunrise_string());
      set_sunset_string(rhs.get_sunset_string());
      set_current_precipitation_string(rhs.get_current_precipitation_string());
    }

    NSPanelWeatherUpdate(const NSPanelWeatherUpdate&& rhs ) noexcept
    {
      set_forecast_items(rhs.get_forecast_items());
      set_current_weather_icon(rhs.get_current_weather_icon());
      set_current_temperature_string(rhs.get_current_temperature_string());
      set_current_maxmin_temperature(rhs.get_current_maxmin_temperature());
      set_current_wind_string(rhs.get_current_wind_string());
      set_sunrise_string(rhs.get_sunrise_string());
      set_sunset_string(rhs.get_sunset_string());
      set_current_precipitation_string(rhs.get_current_precipitation_string());
    }

    ~NSPanelWeatherUpdate() override = default;

    template<
        uint32_t ForecastItem_weather_icon_LENGTH, 
        uint32_t ForecastItem_precipitation_string_LENGTH, 
        uint32_t ForecastItem_temperature_maxmin_string_LENGTH, 
        uint32_t ForecastItem_wind_string_LENGTH, 
        uint32_t ForecastItem_display_string_LENGTH
    >
    class ForecastItem final: public ::EmbeddedProto::MessageInterface
    {
      public:
        ForecastItem() = default;
        ForecastItem(const ForecastItem& rhs )
        {
          set_weather_icon(rhs.get_weather_icon());
          set_precipitation_string(rhs.get_precipitation_string());
          set_temperature_maxmin_string(rhs.get_temperature_maxmin_string());
          set_wind_string(rhs.get_wind_string());
          set_display_string(rhs.get_display_string());
        }

        ForecastItem(const ForecastItem&& rhs ) noexcept
        {
          set_weather_icon(rhs.get_weather_icon());
          set_precipitation_string(rhs.get_precipitation_string());
          set_temperature_maxmin_string(rhs.get_temperature_maxmin_string());
          set_wind_string(rhs.get_wind_string());
          set_display_string(rhs.get_display_string());
        }

        ~ForecastItem() override = default;

        enum class FieldNumber : uint32_t
        {
          NOT_SET = 0,
          WEATHER_ICON = 1,
          PRECIPITATION_STRING = 2,
          TEMPERATURE_MAXMIN_STRING = 3,
          WIND_STRING = 4,
          DISPLAY_STRING = 5
        };

        ForecastItem& operator=(const ForecastItem& rhs)
        {
          set_weather_icon(rhs.get_weather_icon());
          set_precipitation_string(rhs.get_precipitation_string());
          set_temperature_maxmin_string(rhs.get_temperature_maxmin_string());
          set_wind_string(rhs.get_wind_string());
          set_display_string(rhs.get_display_string());
          return *this;
        }

        ForecastItem& operator=(const ForecastItem&& rhs) noexcept
        {
          set_weather_icon(rhs.get_weather_icon());
          set_precipitation_string(rhs.get_precipitation_string());
          set_temperature_maxmin_string(rhs.get_temperature_maxmin_string());
          set_wind_string(rhs.get_wind_string());
          set_display_string(rhs.get_display_string());
          return *this;
        }

        static constexpr char const* WEATHER_ICON_NAME = "weather_icon";
        inline void clear_weather_icon() { weather_icon_.clear(); }
        inline ::EmbeddedProto::FieldString<ForecastItem_weather_icon_LENGTH>& mutable_weather_icon() { return weather_icon_; }
        inline void set_weather_icon(const ::EmbeddedProto::FieldString<ForecastItem_weather_icon_LENGTH>& rhs) { weather_icon_.set(rhs); }
        inline const ::EmbeddedProto::FieldString<ForecastItem_weather_icon_LENGTH>& get_weather_icon() const { return weather_icon_; }
        inline const char* weather_icon() const { return weather_icon_.get_const(); }

        static constexpr char const* PRECIPITATION_STRING_NAME = "precipitation_string";
        inline void clear_precipitation_string() { precipitation_string_.clear(); }
        inline ::EmbeddedProto::FieldString<ForecastItem_precipitation_string_LENGTH>& mutable_precipitation_string() { return precipitation_string_; }
        inline void set_precipitation_string(const ::EmbeddedProto::FieldString<ForecastItem_precipitation_string_LENGTH>& rhs) { precipitation_string_.set(rhs); }
        inline const ::EmbeddedProto::FieldString<ForecastItem_precipitation_string_LENGTH>& get_precipitation_string() const { return precipitation_string_; }
        inline const char* precipitation_string() const { return precipitation_string_.get_const(); }

        static constexpr char const* TEMPERATURE_MAXMIN_STRING_NAME = "temperature_maxmin_string";
        inline void clear_temperature_maxmin_string() { temperature_maxmin_string_.clear(); }
        inline ::EmbeddedProto::FieldString<ForecastItem_temperature_maxmin_string_LENGTH>& mutable_temperature_maxmin_string() { return temperature_maxmin_string_; }
        inline void set_temperature_maxmin_string(const ::EmbeddedProto::FieldString<ForecastItem_temperature_maxmin_string_LENGTH>& rhs) { temperature_maxmin_string_.set(rhs); }
        inline const ::EmbeddedProto::FieldString<ForecastItem_temperature_maxmin_string_LENGTH>& get_temperature_maxmin_string() const { return temperature_maxmin_string_; }
        inline const char* temperature_maxmin_string() const { return temperature_maxmin_string_.get_const(); }

        static constexpr char const* WIND_STRING_NAME = "wind_string";
        inline void clear_wind_string() { wind_string_.clear(); }
        inline ::EmbeddedProto::FieldString<ForecastItem_wind_string_LENGTH>& mutable_wind_string() { return wind_string_; }
        inline void set_wind_string(const ::EmbeddedProto::FieldString<ForecastItem_wind_string_LENGTH>& rhs) { wind_string_.set(rhs); }
        inline const ::EmbeddedProto::FieldString<ForecastItem_wind_string_LENGTH>& get_wind_string() const { return wind_string_; }
        inline const char* wind_string() const { return wind_string_.get_const(); }

        static constexpr char const* DISPLAY_STRING_NAME = "display_string";
        inline void clear_display_string() { display_string_.clear(); }
        inline ::EmbeddedProto::FieldString<ForecastItem_display_string_LENGTH>& mutable_display_string() { return display_string_; }
        inline void set_display_string(const ::EmbeddedProto::FieldString<ForecastItem_display_string_LENGTH>& rhs) { display_string_.set(rhs); }
        inline const ::EmbeddedProto::FieldString<ForecastItem_display_string_LENGTH>& get_display_string() const { return display_string_; }
        inline const char* display_string() const { return display_string_.get_const(); }


        ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = weather_icon_.serialize_with_id(static_cast<uint32_t>(FieldNumber::WEATHER_ICON), buffer, false);
          }

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = precipitation_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::PRECIPITATION_STRING), buffer, false);
          }

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = temperature_maxmin_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TEMPERATURE_MAXMIN_STRING), buffer, false);
          }

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = wind_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::WIND_STRING), buffer, false);
          }

          if(::EmbeddedProto::Error::NO_ERRORS == return_value)
          {
            return_value = display_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DISPLAY_STRING), buffer, false);
          }

          return return_value;
        };

        ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
        {
          ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
          ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
          uint32_t id_number = 0;
          FieldNumber id_tag = FieldNumber::NOT_SET;

          ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
          while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
          {
            id_tag = static_cast<FieldNumber>(id_number);
            switch(id_tag)
            {
              case FieldNumber::WEATHER_ICON:
                return_value = weather_icon_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::PRECIPITATION_STRING:
                return_value = precipitation_string_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::TEMPERATURE_MAXMIN_STRING:
                return_value = temperature_maxmin_string_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::WIND_STRING:
                return_value = wind_string_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::DISPLAY_STRING:
                return_value = display_string_.deserialize_check_type(buffer, wire_type);
                break;

              case FieldNumber::NOT_SET:
                return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
                break;

              default:
                return_value = skip_unknown_field(buffer, wire_type);
                break;
            }

            if(::EmbeddedProto::Error::NO_ERRORS == return_value)
            {
              // Read the next tag.
              tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
            }
          }

          // When an error was detect while reading the tag but no other errors where found, set it in the return value.
          if((::EmbeddedProto::Error::NO_ERRORS == return_value)
             && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
             && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
          {
            return_value = tag_value;
          }

          return return_value;
        };

        void clear() override
        {
          clear_weather_icon();
          clear_precipitation_string();
          clear_temperature_maxmin_string();
          clear_wind_string();
          clear_display_string();

        }

        static char const* field_number_to_name(const FieldNumber fieldNumber)
        {
          char const* name = nullptr;
          switch(fieldNumber)
          {
            case FieldNumber::WEATHER_ICON:
              name = WEATHER_ICON_NAME;
              break;
            case FieldNumber::PRECIPITATION_STRING:
              name = PRECIPITATION_STRING_NAME;
              break;
            case FieldNumber::TEMPERATURE_MAXMIN_STRING:
              name = TEMPERATURE_MAXMIN_STRING_NAME;
              break;
            case FieldNumber::WIND_STRING:
              name = WIND_STRING_NAME;
              break;
            case FieldNumber::DISPLAY_STRING:
              name = DISPLAY_STRING_NAME;
              break;
            default:
              name = "Invalid FieldNumber";
              break;
          }
          return name;
        }

    #ifdef MSG_TO_STRING

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
        {
          return this->to_string(str, 0, nullptr, true);
        }

        ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
        {
          ::EmbeddedProto::string_view left_chars = str;
          int32_t n_chars_used = 0;

          if(!first_field)
          {
            // Add a comma behind the previous field.
            n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
            if(0 < n_chars_used)
            {
              // Update the character pointer and characters left in the array.
              left_chars.data += n_chars_used;
              left_chars.size -= n_chars_used;
            }
          }

          if(nullptr != name)
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
            }
          }
          else
          {
            if( 0 == indent_level)
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
            }
            else
            {
              n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
            }
          }
          
          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          left_chars = weather_icon_.to_string(left_chars, indent_level + 2, WEATHER_ICON_NAME, true);
          left_chars = precipitation_string_.to_string(left_chars, indent_level + 2, PRECIPITATION_STRING_NAME, false);
          left_chars = temperature_maxmin_string_.to_string(left_chars, indent_level + 2, TEMPERATURE_MAXMIN_STRING_NAME, false);
          left_chars = wind_string_.to_string(left_chars, indent_level + 2, WIND_STRING_NAME, false);
          left_chars = display_string_.to_string(left_chars, indent_level + 2, DISPLAY_STRING_NAME, false);
      
          if( 0 == indent_level) 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
          }
          else 
          {
            n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
          }

          if(0 < n_chars_used)
          {
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }

          return left_chars;
        }

    #endif // End of MSG_TO_STRING

      private:


          ::EmbeddedProto::FieldString<ForecastItem_weather_icon_LENGTH> weather_icon_;
          ::EmbeddedProto::FieldString<ForecastItem_precipitation_string_LENGTH> precipitation_string_;
          ::EmbeddedProto::FieldString<ForecastItem_temperature_maxmin_string_LENGTH> temperature_maxmin_string_;
          ::EmbeddedProto::FieldString<ForecastItem_wind_string_LENGTH> wind_string_;
          ::EmbeddedProto::FieldString<ForecastItem_display_string_LENGTH> display_string_;

    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      FORECAST_ITEMS = 1,
      CURRENT_WEATHER_ICON = 2,
      CURRENT_TEMPERATURE_STRING = 3,
      CURRENT_MAXMIN_TEMPERATURE = 4,
      CURRENT_WIND_STRING = 5,
      SUNRISE_STRING = 6,
      SUNSET_STRING = 7,
      CURRENT_PRECIPITATION_STRING = 8
    };

    NSPanelWeatherUpdate& operator=(const NSPanelWeatherUpdate& rhs)
    {
      set_forecast_items(rhs.get_forecast_items());
      set_current_weather_icon(rhs.get_current_weather_icon());
      set_current_temperature_string(rhs.get_current_temperature_string());
      set_current_maxmin_temperature(rhs.get_current_maxmin_temperature());
      set_current_wind_string(rhs.get_current_wind_string());
      set_sunrise_string(rhs.get_sunrise_string());
      set_sunset_string(rhs.get_sunset_string());
      set_current_precipitation_string(rhs.get_current_precipitation_string());
      return *this;
    }

    NSPanelWeatherUpdate& operator=(const NSPanelWeatherUpdate&& rhs) noexcept
    {
      set_forecast_items(rhs.get_forecast_items());
      set_current_weather_icon(rhs.get_current_weather_icon());
      set_current_temperature_string(rhs.get_current_temperature_string());
      set_current_maxmin_temperature(rhs.get_current_maxmin_temperature());
      set_current_wind_string(rhs.get_current_wind_string());
      set_sunrise_string(rhs.get_sunrise_string());
      set_sunset_string(rhs.get_sunset_string());
      set_current_precipitation_string(rhs.get_current_precipitation_string());
      return *this;
    }

    static constexpr char const* FORECAST_ITEMS_NAME = "forecast_items";
    inline const ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>& forecast_items(uint32_t index) const { return forecast_items_[index]; }
    inline void clear_forecast_items() { forecast_items_.clear(); }
    inline void set_forecast_items(uint32_t index, const ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>& value) { forecast_items_.set(index, value); }
    inline void set_forecast_items(uint32_t index, const ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>&& value) { forecast_items_.set(index, value); }
    inline void set_forecast_items(const ::EmbeddedProto::RepeatedFieldFixedSize<ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>, NSPanelWeatherUpdate_forecast_items_REP_LENGTH>& values) { forecast_items_ = values; }
    inline void add_forecast_items(const ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>& value) { forecast_items_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>, NSPanelWeatherUpdate_forecast_items_REP_LENGTH>& mutable_forecast_items() { return forecast_items_; }
    inline ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>& mutable_forecast_items(uint32_t index) { return forecast_items_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>, NSPanelWeatherUpdate_forecast_items_REP_LENGTH>& get_forecast_items() const { return forecast_items_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>, NSPanelWeatherUpdate_forecast_items_REP_LENGTH>& forecast_items() const { return forecast_items_; }

    static constexpr char const* CURRENT_WEATHER_ICON_NAME = "current_weather_icon";
    inline void clear_current_weather_icon() { current_weather_icon_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_weather_icon_LENGTH>& mutable_current_weather_icon() { return current_weather_icon_; }
    inline void set_current_weather_icon(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_weather_icon_LENGTH>& rhs) { current_weather_icon_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_weather_icon_LENGTH>& get_current_weather_icon() const { return current_weather_icon_; }
    inline const char* current_weather_icon() const { return current_weather_icon_.get_const(); }

    static constexpr char const* CURRENT_TEMPERATURE_STRING_NAME = "current_temperature_string";
    inline void clear_current_temperature_string() { current_temperature_string_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_temperature_string_LENGTH>& mutable_current_temperature_string() { return current_temperature_string_; }
    inline void set_current_temperature_string(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_temperature_string_LENGTH>& rhs) { current_temperature_string_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_temperature_string_LENGTH>& get_current_temperature_string() const { return current_temperature_string_; }
    inline const char* current_temperature_string() const { return current_temperature_string_.get_const(); }

    static constexpr char const* CURRENT_MAXMIN_TEMPERATURE_NAME = "current_maxmin_temperature";
    inline void clear_current_maxmin_temperature() { current_maxmin_temperature_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_maxmin_temperature_LENGTH>& mutable_current_maxmin_temperature() { return current_maxmin_temperature_; }
    inline void set_current_maxmin_temperature(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_maxmin_temperature_LENGTH>& rhs) { current_maxmin_temperature_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_maxmin_temperature_LENGTH>& get_current_maxmin_temperature() const { return current_maxmin_temperature_; }
    inline const char* current_maxmin_temperature() const { return current_maxmin_temperature_.get_const(); }

    static constexpr char const* CURRENT_WIND_STRING_NAME = "current_wind_string";
    inline void clear_current_wind_string() { current_wind_string_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_wind_string_LENGTH>& mutable_current_wind_string() { return current_wind_string_; }
    inline void set_current_wind_string(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_wind_string_LENGTH>& rhs) { current_wind_string_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_wind_string_LENGTH>& get_current_wind_string() const { return current_wind_string_; }
    inline const char* current_wind_string() const { return current_wind_string_.get_const(); }

    static constexpr char const* SUNRISE_STRING_NAME = "sunrise_string";
    inline void clear_sunrise_string() { sunrise_string_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunrise_string_LENGTH>& mutable_sunrise_string() { return sunrise_string_; }
    inline void set_sunrise_string(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunrise_string_LENGTH>& rhs) { sunrise_string_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunrise_string_LENGTH>& get_sunrise_string() const { return sunrise_string_; }
    inline const char* sunrise_string() const { return sunrise_string_.get_const(); }

    static constexpr char const* SUNSET_STRING_NAME = "sunset_string";
    inline void clear_sunset_string() { sunset_string_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunset_string_LENGTH>& mutable_sunset_string() { return sunset_string_; }
    inline void set_sunset_string(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunset_string_LENGTH>& rhs) { sunset_string_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunset_string_LENGTH>& get_sunset_string() const { return sunset_string_; }
    inline const char* sunset_string() const { return sunset_string_.get_const(); }

    static constexpr char const* CURRENT_PRECIPITATION_STRING_NAME = "current_precipitation_string";
    inline void clear_current_precipitation_string() { current_precipitation_string_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_precipitation_string_LENGTH>& mutable_current_precipitation_string() { return current_precipitation_string_; }
    inline void set_current_precipitation_string(const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_precipitation_string_LENGTH>& rhs) { current_precipitation_string_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_precipitation_string_LENGTH>& get_current_precipitation_string() const { return current_precipitation_string_; }
    inline const char* current_precipitation_string() const { return current_precipitation_string_.get_const(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = forecast_items_.serialize_with_id(static_cast<uint32_t>(FieldNumber::FORECAST_ITEMS), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = current_weather_icon_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CURRENT_WEATHER_ICON), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = current_temperature_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CURRENT_TEMPERATURE_STRING), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = current_maxmin_temperature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CURRENT_MAXMIN_TEMPERATURE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = current_wind_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CURRENT_WIND_STRING), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = sunrise_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUNRISE_STRING), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = sunset_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SUNSET_STRING), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = current_precipitation_string_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CURRENT_PRECIPITATION_STRING), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::FORECAST_ITEMS:
            return_value = forecast_items_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CURRENT_WEATHER_ICON:
            return_value = current_weather_icon_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CURRENT_TEMPERATURE_STRING:
            return_value = current_temperature_string_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CURRENT_MAXMIN_TEMPERATURE:
            return_value = current_maxmin_temperature_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CURRENT_WIND_STRING:
            return_value = current_wind_string_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SUNRISE_STRING:
            return_value = sunrise_string_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SUNSET_STRING:
            return_value = sunset_string_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CURRENT_PRECIPITATION_STRING:
            return_value = current_precipitation_string_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_forecast_items();
      clear_current_weather_icon();
      clear_current_temperature_string();
      clear_current_maxmin_temperature();
      clear_current_wind_string();
      clear_sunrise_string();
      clear_sunset_string();
      clear_current_precipitation_string();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::FORECAST_ITEMS:
          name = FORECAST_ITEMS_NAME;
          break;
        case FieldNumber::CURRENT_WEATHER_ICON:
          name = CURRENT_WEATHER_ICON_NAME;
          break;
        case FieldNumber::CURRENT_TEMPERATURE_STRING:
          name = CURRENT_TEMPERATURE_STRING_NAME;
          break;
        case FieldNumber::CURRENT_MAXMIN_TEMPERATURE:
          name = CURRENT_MAXMIN_TEMPERATURE_NAME;
          break;
        case FieldNumber::CURRENT_WIND_STRING:
          name = CURRENT_WIND_STRING_NAME;
          break;
        case FieldNumber::SUNRISE_STRING:
          name = SUNRISE_STRING_NAME;
          break;
        case FieldNumber::SUNSET_STRING:
          name = SUNSET_STRING_NAME;
          break;
        case FieldNumber::CURRENT_PRECIPITATION_STRING:
          name = CURRENT_PRECIPITATION_STRING_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = forecast_items_.to_string(left_chars, indent_level + 2, FORECAST_ITEMS_NAME, true);
      left_chars = current_weather_icon_.to_string(left_chars, indent_level + 2, CURRENT_WEATHER_ICON_NAME, false);
      left_chars = current_temperature_string_.to_string(left_chars, indent_level + 2, CURRENT_TEMPERATURE_STRING_NAME, false);
      left_chars = current_maxmin_temperature_.to_string(left_chars, indent_level + 2, CURRENT_MAXMIN_TEMPERATURE_NAME, false);
      left_chars = current_wind_string_.to_string(left_chars, indent_level + 2, CURRENT_WIND_STRING_NAME, false);
      left_chars = sunrise_string_.to_string(left_chars, indent_level + 2, SUNRISE_STRING_NAME, false);
      left_chars = sunset_string_.to_string(left_chars, indent_level + 2, SUNSET_STRING_NAME, false);
      left_chars = current_precipitation_string_.to_string(left_chars, indent_level + 2, CURRENT_PRECIPITATION_STRING_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      ::EmbeddedProto::RepeatedFieldFixedSize<ForecastItem<NSPanelWeatherUpdate_forecast_items_ForecastItem_weather_icon_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_precipitation_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_temperature_maxmin_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_wind_string_LENGTH, NSPanelWeatherUpdate_forecast_items_ForecastItem_display_string_LENGTH>, NSPanelWeatherUpdate_forecast_items_REP_LENGTH> forecast_items_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_weather_icon_LENGTH> current_weather_icon_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_temperature_string_LENGTH> current_temperature_string_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_maxmin_temperature_LENGTH> current_maxmin_temperature_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_wind_string_LENGTH> current_wind_string_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunrise_string_LENGTH> sunrise_string_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_sunset_string_LENGTH> sunset_string_;
      ::EmbeddedProto::FieldString<NSPanelWeatherUpdate_current_precipitation_string_LENGTH> current_precipitation_string_;

};

template<
    uint32_t NSPanelConfig_name_LENGTH, 
    uint32_t NSPanelConfig_room_ids_REP_LENGTH, 
    uint32_t NSPanelConfig_button1_mqtt_topic_LENGTH, 
    uint32_t NSPanelConfig_button1_mqtt_payload_LENGTH, 
    uint32_t NSPanelConfig_button2_mqtt_topic_LENGTH, 
    uint32_t NSPanelConfig_button2_mqtt_payload_LENGTH, 
    uint32_t NSPanelConfig_global_scenes_REP_LENGTH, 
    uint32_t NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH
>
class NSPanelConfig final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelConfig() = default;
    NSPanelConfig(const NSPanelConfig& rhs )
    {
      set_name(rhs.get_name());
      set_default_room(rhs.get_default_room());
      set_default_page(rhs.get_default_page());
      set_screensaver_activation_timeout(rhs.get_screensaver_activation_timeout());
      set_min_button_push_time(rhs.get_min_button_push_time());
      set_button_long_press_time(rhs.get_button_long_press_time());
      set_special_mode_trigger_time(rhs.get_special_mode_trigger_time());
      set_special_mode_release_time(rhs.get_special_mode_release_time());
      set_screen_dim_level(rhs.get_screen_dim_level());
      set_screensaver_dim_level(rhs.get_screensaver_dim_level());
      set_screensaver_mode(rhs.get_screensaver_mode());
      set_clock_us_style(rhs.get_clock_us_style());
      set_use_fahrenheit(rhs.get_use_fahrenheit());
      set_is_us_panel(rhs.get_is_us_panel());
      set_room_ids(rhs.get_room_ids());
      set_reverse_relays(rhs.get_reverse_relays());
      set_relay1_default_mode(rhs.get_relay1_default_mode());
      set_relay2_default_mode(rhs.get_relay2_default_mode());
      set_temperature_calibration(rhs.get_temperature_calibration());
      set_button1_mode(rhs.get_button1_mode());
      set_button1_mqtt_topic(rhs.get_button1_mqtt_topic());
      set_button1_mqtt_payload(rhs.get_button1_mqtt_payload());
      set_button2_mode(rhs.get_button2_mode());
      set_button2_mqtt_topic(rhs.get_button2_mqtt_topic());
      set_button2_mqtt_payload(rhs.get_button2_mqtt_payload());
      set_button1_detached_light_id(rhs.get_button1_detached_light_id());
      set_button2_detached_light_id(rhs.get_button2_detached_light_id());
      set_global_scenes(rhs.get_global_scenes());
      set_optimistic_mode(rhs.get_optimistic_mode());
    }

    NSPanelConfig(const NSPanelConfig&& rhs ) noexcept
    {
      set_name(rhs.get_name());
      set_default_room(rhs.get_default_room());
      set_default_page(rhs.get_default_page());
      set_screensaver_activation_timeout(rhs.get_screensaver_activation_timeout());
      set_min_button_push_time(rhs.get_min_button_push_time());
      set_button_long_press_time(rhs.get_button_long_press_time());
      set_special_mode_trigger_time(rhs.get_special_mode_trigger_time());
      set_special_mode_release_time(rhs.get_special_mode_release_time());
      set_screen_dim_level(rhs.get_screen_dim_level());
      set_screensaver_dim_level(rhs.get_screensaver_dim_level());
      set_screensaver_mode(rhs.get_screensaver_mode());
      set_clock_us_style(rhs.get_clock_us_style());
      set_use_fahrenheit(rhs.get_use_fahrenheit());
      set_is_us_panel(rhs.get_is_us_panel());
      set_room_ids(rhs.get_room_ids());
      set_reverse_relays(rhs.get_reverse_relays());
      set_relay1_default_mode(rhs.get_relay1_default_mode());
      set_relay2_default_mode(rhs.get_relay2_default_mode());
      set_temperature_calibration(rhs.get_temperature_calibration());
      set_button1_mode(rhs.get_button1_mode());
      set_button1_mqtt_topic(rhs.get_button1_mqtt_topic());
      set_button1_mqtt_payload(rhs.get_button1_mqtt_payload());
      set_button2_mode(rhs.get_button2_mode());
      set_button2_mqtt_topic(rhs.get_button2_mqtt_topic());
      set_button2_mqtt_payload(rhs.get_button2_mqtt_payload());
      set_button1_detached_light_id(rhs.get_button1_detached_light_id());
      set_button2_detached_light_id(rhs.get_button2_detached_light_id());
      set_global_scenes(rhs.get_global_scenes());
      set_optimistic_mode(rhs.get_optimistic_mode());
    }

    ~NSPanelConfig() override = default;

    enum class NSPanelScreensaverMode : uint32_t
    {
      WEATHER_WITH_BACKGROUND = 0,
      WEATHER_WITHOUT_BACKGROUND = 1,
      DATETIME_WITH_BACKGROUND = 3,
      DATETIME_WITHOUT_BACKGROUND = 4,
      NO_SCREENSAVER = 5
    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      NAME = 1,
      DEFAULT_ROOM = 2,
      DEFAULT_PAGE = 3,
      SCREENSAVER_ACTIVATION_TIMEOUT = 4,
      MIN_BUTTON_PUSH_TIME = 5,
      BUTTON_LONG_PRESS_TIME = 6,
      SPECIAL_MODE_TRIGGER_TIME = 7,
      SPECIAL_MODE_RELEASE_TIME = 8,
      SCREEN_DIM_LEVEL = 9,
      SCREENSAVER_DIM_LEVEL = 10,
      SCREENSAVER_MODE = 11,
      CLOCK_US_STYLE = 12,
      USE_FAHRENHEIT = 13,
      IS_US_PANEL = 14,
      ROOM_IDS = 15,
      REVERSE_RELAYS = 16,
      RELAY1_DEFAULT_MODE = 17,
      RELAY2_DEFAULT_MODE = 18,
      TEMPERATURE_CALIBRATION = 19,
      BUTTON1_MODE = 20,
      BUTTON1_MQTT_TOPIC = 21,
      BUTTON1_MQTT_PAYLOAD = 22,
      BUTTON2_MODE = 23,
      BUTTON2_MQTT_TOPIC = 24,
      BUTTON2_MQTT_PAYLOAD = 25,
      BUTTON1_DETACHED_LIGHT_ID = 26,
      BUTTON2_DETACHED_LIGHT_ID = 27,
      GLOBAL_SCENES = 28,
      OPTIMISTIC_MODE = 29
    };

    NSPanelConfig& operator=(const NSPanelConfig& rhs)
    {
      set_name(rhs.get_name());
      set_default_room(rhs.get_default_room());
      set_default_page(rhs.get_default_page());
      set_screensaver_activation_timeout(rhs.get_screensaver_activation_timeout());
      set_min_button_push_time(rhs.get_min_button_push_time());
      set_button_long_press_time(rhs.get_button_long_press_time());
      set_special_mode_trigger_time(rhs.get_special_mode_trigger_time());
      set_special_mode_release_time(rhs.get_special_mode_release_time());
      set_screen_dim_level(rhs.get_screen_dim_level());
      set_screensaver_dim_level(rhs.get_screensaver_dim_level());
      set_screensaver_mode(rhs.get_screensaver_mode());
      set_clock_us_style(rhs.get_clock_us_style());
      set_use_fahrenheit(rhs.get_use_fahrenheit());
      set_is_us_panel(rhs.get_is_us_panel());
      set_room_ids(rhs.get_room_ids());
      set_reverse_relays(rhs.get_reverse_relays());
      set_relay1_default_mode(rhs.get_relay1_default_mode());
      set_relay2_default_mode(rhs.get_relay2_default_mode());
      set_temperature_calibration(rhs.get_temperature_calibration());
      set_button1_mode(rhs.get_button1_mode());
      set_button1_mqtt_topic(rhs.get_button1_mqtt_topic());
      set_button1_mqtt_payload(rhs.get_button1_mqtt_payload());
      set_button2_mode(rhs.get_button2_mode());
      set_button2_mqtt_topic(rhs.get_button2_mqtt_topic());
      set_button2_mqtt_payload(rhs.get_button2_mqtt_payload());
      set_button1_detached_light_id(rhs.get_button1_detached_light_id());
      set_button2_detached_light_id(rhs.get_button2_detached_light_id());
      set_global_scenes(rhs.get_global_scenes());
      set_optimistic_mode(rhs.get_optimistic_mode());
      return *this;
    }

    NSPanelConfig& operator=(const NSPanelConfig&& rhs) noexcept
    {
      set_name(rhs.get_name());
      set_default_room(rhs.get_default_room());
      set_default_page(rhs.get_default_page());
      set_screensaver_activation_timeout(rhs.get_screensaver_activation_timeout());
      set_min_button_push_time(rhs.get_min_button_push_time());
      set_button_long_press_time(rhs.get_button_long_press_time());
      set_special_mode_trigger_time(rhs.get_special_mode_trigger_time());
      set_special_mode_release_time(rhs.get_special_mode_release_time());
      set_screen_dim_level(rhs.get_screen_dim_level());
      set_screensaver_dim_level(rhs.get_screensaver_dim_level());
      set_screensaver_mode(rhs.get_screensaver_mode());
      set_clock_us_style(rhs.get_clock_us_style());
      set_use_fahrenheit(rhs.get_use_fahrenheit());
      set_is_us_panel(rhs.get_is_us_panel());
      set_room_ids(rhs.get_room_ids());
      set_reverse_relays(rhs.get_reverse_relays());
      set_relay1_default_mode(rhs.get_relay1_default_mode());
      set_relay2_default_mode(rhs.get_relay2_default_mode());
      set_temperature_calibration(rhs.get_temperature_calibration());
      set_button1_mode(rhs.get_button1_mode());
      set_button1_mqtt_topic(rhs.get_button1_mqtt_topic());
      set_button1_mqtt_payload(rhs.get_button1_mqtt_payload());
      set_button2_mode(rhs.get_button2_mode());
      set_button2_mqtt_topic(rhs.get_button2_mqtt_topic());
      set_button2_mqtt_payload(rhs.get_button2_mqtt_payload());
      set_button1_detached_light_id(rhs.get_button1_detached_light_id());
      set_button2_detached_light_id(rhs.get_button2_detached_light_id());
      set_global_scenes(rhs.get_global_scenes());
      set_optimistic_mode(rhs.get_optimistic_mode());
      return *this;
    }

    static constexpr char const* NAME_NAME = "name";
    inline void clear_name() { name_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelConfig_name_LENGTH>& mutable_name() { return name_; }
    inline void set_name(const ::EmbeddedProto::FieldString<NSPanelConfig_name_LENGTH>& rhs) { name_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelConfig_name_LENGTH>& get_name() const { return name_; }
    inline const char* name() const { return name_.get_const(); }

    static constexpr char const* DEFAULT_ROOM_NAME = "default_room";
    inline void clear_default_room() { default_room_.clear(); }
    inline void set_default_room(const int32_t& value) { default_room_ = value; }
    inline void set_default_room(const int32_t&& value) { default_room_ = value; }
    inline int32_t& mutable_default_room() { return default_room_.get(); }
    inline const int32_t& get_default_room() const { return default_room_.get(); }
    inline int32_t default_room() const { return default_room_.get(); }

    static constexpr char const* DEFAULT_PAGE_NAME = "default_page";
    inline void clear_default_page() { default_page_.clear(); }
    inline void set_default_page(const int32_t& value) { default_page_ = value; }
    inline void set_default_page(const int32_t&& value) { default_page_ = value; }
    inline int32_t& mutable_default_page() { return default_page_.get(); }
    inline const int32_t& get_default_page() const { return default_page_.get(); }
    inline int32_t default_page() const { return default_page_.get(); }

    static constexpr char const* SCREENSAVER_ACTIVATION_TIMEOUT_NAME = "screensaver_activation_timeout";
    inline void clear_screensaver_activation_timeout() { screensaver_activation_timeout_.clear(); }
    inline void set_screensaver_activation_timeout(const int32_t& value) { screensaver_activation_timeout_ = value; }
    inline void set_screensaver_activation_timeout(const int32_t&& value) { screensaver_activation_timeout_ = value; }
    inline int32_t& mutable_screensaver_activation_timeout() { return screensaver_activation_timeout_.get(); }
    inline const int32_t& get_screensaver_activation_timeout() const { return screensaver_activation_timeout_.get(); }
    inline int32_t screensaver_activation_timeout() const { return screensaver_activation_timeout_.get(); }

    static constexpr char const* MIN_BUTTON_PUSH_TIME_NAME = "min_button_push_time";
    inline void clear_min_button_push_time() { min_button_push_time_.clear(); }
    inline void set_min_button_push_time(const int32_t& value) { min_button_push_time_ = value; }
    inline void set_min_button_push_time(const int32_t&& value) { min_button_push_time_ = value; }
    inline int32_t& mutable_min_button_push_time() { return min_button_push_time_.get(); }
    inline const int32_t& get_min_button_push_time() const { return min_button_push_time_.get(); }
    inline int32_t min_button_push_time() const { return min_button_push_time_.get(); }

    static constexpr char const* BUTTON_LONG_PRESS_TIME_NAME = "button_long_press_time";
    inline void clear_button_long_press_time() { button_long_press_time_.clear(); }
    inline void set_button_long_press_time(const int32_t& value) { button_long_press_time_ = value; }
    inline void set_button_long_press_time(const int32_t&& value) { button_long_press_time_ = value; }
    inline int32_t& mutable_button_long_press_time() { return button_long_press_time_.get(); }
    inline const int32_t& get_button_long_press_time() const { return button_long_press_time_.get(); }
    inline int32_t button_long_press_time() const { return button_long_press_time_.get(); }

    static constexpr char const* SPECIAL_MODE_TRIGGER_TIME_NAME = "special_mode_trigger_time";
    inline void clear_special_mode_trigger_time() { special_mode_trigger_time_.clear(); }
    inline void set_special_mode_trigger_time(const int32_t& value) { special_mode_trigger_time_ = value; }
    inline void set_special_mode_trigger_time(const int32_t&& value) { special_mode_trigger_time_ = value; }
    inline int32_t& mutable_special_mode_trigger_time() { return special_mode_trigger_time_.get(); }
    inline const int32_t& get_special_mode_trigger_time() const { return special_mode_trigger_time_.get(); }
    inline int32_t special_mode_trigger_time() const { return special_mode_trigger_time_.get(); }

    static constexpr char const* SPECIAL_MODE_RELEASE_TIME_NAME = "special_mode_release_time";
    inline void clear_special_mode_release_time() { special_mode_release_time_.clear(); }
    inline void set_special_mode_release_time(const int32_t& value) { special_mode_release_time_ = value; }
    inline void set_special_mode_release_time(const int32_t&& value) { special_mode_release_time_ = value; }
    inline int32_t& mutable_special_mode_release_time() { return special_mode_release_time_.get(); }
    inline const int32_t& get_special_mode_release_time() const { return special_mode_release_time_.get(); }
    inline int32_t special_mode_release_time() const { return special_mode_release_time_.get(); }

    static constexpr char const* SCREEN_DIM_LEVEL_NAME = "screen_dim_level";
    inline void clear_screen_dim_level() { screen_dim_level_.clear(); }
    inline void set_screen_dim_level(const int32_t& value) { screen_dim_level_ = value; }
    inline void set_screen_dim_level(const int32_t&& value) { screen_dim_level_ = value; }
    inline int32_t& mutable_screen_dim_level() { return screen_dim_level_.get(); }
    inline const int32_t& get_screen_dim_level() const { return screen_dim_level_.get(); }
    inline int32_t screen_dim_level() const { return screen_dim_level_.get(); }

    static constexpr char const* SCREENSAVER_DIM_LEVEL_NAME = "screensaver_dim_level";
    inline void clear_screensaver_dim_level() { screensaver_dim_level_.clear(); }
    inline void set_screensaver_dim_level(const int32_t& value) { screensaver_dim_level_ = value; }
    inline void set_screensaver_dim_level(const int32_t&& value) { screensaver_dim_level_ = value; }
    inline int32_t& mutable_screensaver_dim_level() { return screensaver_dim_level_.get(); }
    inline const int32_t& get_screensaver_dim_level() const { return screensaver_dim_level_.get(); }
    inline int32_t screensaver_dim_level() const { return screensaver_dim_level_.get(); }

    static constexpr char const* SCREENSAVER_MODE_NAME = "screensaver_mode";
    inline void clear_screensaver_mode() { screensaver_mode_.clear(); }
    inline void set_screensaver_mode(const NSPanelScreensaverMode& value) { screensaver_mode_ = value; }
    inline void set_screensaver_mode(const NSPanelScreensaverMode&& value) { screensaver_mode_ = value; }
    inline const NSPanelScreensaverMode& get_screensaver_mode() const { return screensaver_mode_.get(); }
    inline NSPanelScreensaverMode screensaver_mode() const { return screensaver_mode_.get(); }

    static constexpr char const* CLOCK_US_STYLE_NAME = "clock_us_style";
    inline void clear_clock_us_style() { clock_us_style_.clear(); }
    inline void set_clock_us_style(const bool& value) { clock_us_style_ = value; }
    inline void set_clock_us_style(const bool&& value) { clock_us_style_ = value; }
    inline bool& mutable_clock_us_style() { return clock_us_style_.get(); }
    inline const bool& get_clock_us_style() const { return clock_us_style_.get(); }
    inline bool clock_us_style() const { return clock_us_style_.get(); }

    static constexpr char const* USE_FAHRENHEIT_NAME = "use_fahrenheit";
    inline void clear_use_fahrenheit() { use_fahrenheit_.clear(); }
    inline void set_use_fahrenheit(const bool& value) { use_fahrenheit_ = value; }
    inline void set_use_fahrenheit(const bool&& value) { use_fahrenheit_ = value; }
    inline bool& mutable_use_fahrenheit() { return use_fahrenheit_.get(); }
    inline const bool& get_use_fahrenheit() const { return use_fahrenheit_.get(); }
    inline bool use_fahrenheit() const { return use_fahrenheit_.get(); }

    static constexpr char const* IS_US_PANEL_NAME = "is_us_panel";
    inline void clear_is_us_panel() { is_us_panel_.clear(); }
    inline void set_is_us_panel(const bool& value) { is_us_panel_ = value; }
    inline void set_is_us_panel(const bool&& value) { is_us_panel_ = value; }
    inline bool& mutable_is_us_panel() { return is_us_panel_.get(); }
    inline const bool& get_is_us_panel() const { return is_us_panel_.get(); }
    inline bool is_us_panel() const { return is_us_panel_.get(); }

    static constexpr char const* ROOM_IDS_NAME = "room_ids";
    inline const EmbeddedProto::int32& room_ids(uint32_t index) const { return room_ids_[index]; }
    inline void clear_room_ids() { room_ids_.clear(); }
    inline void set_room_ids(uint32_t index, const EmbeddedProto::int32& value) { room_ids_.set(index, value); }
    inline void set_room_ids(uint32_t index, const EmbeddedProto::int32&& value) { room_ids_.set(index, value); }
    inline void set_room_ids(const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, NSPanelConfig_room_ids_REP_LENGTH>& values) { room_ids_ = values; }
    inline void add_room_ids(const EmbeddedProto::int32& value) { room_ids_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, NSPanelConfig_room_ids_REP_LENGTH>& mutable_room_ids() { return room_ids_; }
    inline EmbeddedProto::int32& mutable_room_ids(uint32_t index) { return room_ids_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, NSPanelConfig_room_ids_REP_LENGTH>& get_room_ids() const { return room_ids_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, NSPanelConfig_room_ids_REP_LENGTH>& room_ids() const { return room_ids_; }

    static constexpr char const* REVERSE_RELAYS_NAME = "reverse_relays";
    inline void clear_reverse_relays() { reverse_relays_.clear(); }
    inline void set_reverse_relays(const bool& value) { reverse_relays_ = value; }
    inline void set_reverse_relays(const bool&& value) { reverse_relays_ = value; }
    inline bool& mutable_reverse_relays() { return reverse_relays_.get(); }
    inline const bool& get_reverse_relays() const { return reverse_relays_.get(); }
    inline bool reverse_relays() const { return reverse_relays_.get(); }

    static constexpr char const* RELAY1_DEFAULT_MODE_NAME = "relay1_default_mode";
    inline void clear_relay1_default_mode() { relay1_default_mode_.clear(); }
    inline void set_relay1_default_mode(const bool& value) { relay1_default_mode_ = value; }
    inline void set_relay1_default_mode(const bool&& value) { relay1_default_mode_ = value; }
    inline bool& mutable_relay1_default_mode() { return relay1_default_mode_.get(); }
    inline const bool& get_relay1_default_mode() const { return relay1_default_mode_.get(); }
    inline bool relay1_default_mode() const { return relay1_default_mode_.get(); }

    static constexpr char const* RELAY2_DEFAULT_MODE_NAME = "relay2_default_mode";
    inline void clear_relay2_default_mode() { relay2_default_mode_.clear(); }
    inline void set_relay2_default_mode(const bool& value) { relay2_default_mode_ = value; }
    inline void set_relay2_default_mode(const bool&& value) { relay2_default_mode_ = value; }
    inline bool& mutable_relay2_default_mode() { return relay2_default_mode_.get(); }
    inline const bool& get_relay2_default_mode() const { return relay2_default_mode_.get(); }
    inline bool relay2_default_mode() const { return relay2_default_mode_.get(); }

    static constexpr char const* TEMPERATURE_CALIBRATION_NAME = "temperature_calibration";
    inline void clear_temperature_calibration() { temperature_calibration_.clear(); }
    inline void set_temperature_calibration(const float& value) { temperature_calibration_ = value; }
    inline void set_temperature_calibration(const float&& value) { temperature_calibration_ = value; }
    inline float& mutable_temperature_calibration() { return temperature_calibration_.get(); }
    inline const float& get_temperature_calibration() const { return temperature_calibration_.get(); }
    inline float temperature_calibration() const { return temperature_calibration_.get(); }

    static constexpr char const* BUTTON1_MODE_NAME = "button1_mode";
    inline void clear_button1_mode() { button1_mode_.clear(); }
    inline void set_button1_mode(const int32_t& value) { button1_mode_ = value; }
    inline void set_button1_mode(const int32_t&& value) { button1_mode_ = value; }
    inline int32_t& mutable_button1_mode() { return button1_mode_.get(); }
    inline const int32_t& get_button1_mode() const { return button1_mode_.get(); }
    inline int32_t button1_mode() const { return button1_mode_.get(); }

    static constexpr char const* BUTTON1_MQTT_TOPIC_NAME = "button1_mqtt_topic";
    inline void clear_button1_mqtt_topic() { button1_mqtt_topic_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_topic_LENGTH>& mutable_button1_mqtt_topic() { return button1_mqtt_topic_; }
    inline void set_button1_mqtt_topic(const ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_topic_LENGTH>& rhs) { button1_mqtt_topic_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_topic_LENGTH>& get_button1_mqtt_topic() const { return button1_mqtt_topic_; }
    inline const char* button1_mqtt_topic() const { return button1_mqtt_topic_.get_const(); }

    static constexpr char const* BUTTON1_MQTT_PAYLOAD_NAME = "button1_mqtt_payload";
    inline void clear_button1_mqtt_payload() { button1_mqtt_payload_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_payload_LENGTH>& mutable_button1_mqtt_payload() { return button1_mqtt_payload_; }
    inline void set_button1_mqtt_payload(const ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_payload_LENGTH>& rhs) { button1_mqtt_payload_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_payload_LENGTH>& get_button1_mqtt_payload() const { return button1_mqtt_payload_; }
    inline const char* button1_mqtt_payload() const { return button1_mqtt_payload_.get_const(); }

    static constexpr char const* BUTTON2_MODE_NAME = "button2_mode";
    inline void clear_button2_mode() { button2_mode_.clear(); }
    inline void set_button2_mode(const int32_t& value) { button2_mode_ = value; }
    inline void set_button2_mode(const int32_t&& value) { button2_mode_ = value; }
    inline int32_t& mutable_button2_mode() { return button2_mode_.get(); }
    inline const int32_t& get_button2_mode() const { return button2_mode_.get(); }
    inline int32_t button2_mode() const { return button2_mode_.get(); }

    static constexpr char const* BUTTON2_MQTT_TOPIC_NAME = "button2_mqtt_topic";
    inline void clear_button2_mqtt_topic() { button2_mqtt_topic_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_topic_LENGTH>& mutable_button2_mqtt_topic() { return button2_mqtt_topic_; }
    inline void set_button2_mqtt_topic(const ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_topic_LENGTH>& rhs) { button2_mqtt_topic_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_topic_LENGTH>& get_button2_mqtt_topic() const { return button2_mqtt_topic_; }
    inline const char* button2_mqtt_topic() const { return button2_mqtt_topic_.get_const(); }

    static constexpr char const* BUTTON2_MQTT_PAYLOAD_NAME = "button2_mqtt_payload";
    inline void clear_button2_mqtt_payload() { button2_mqtt_payload_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_payload_LENGTH>& mutable_button2_mqtt_payload() { return button2_mqtt_payload_; }
    inline void set_button2_mqtt_payload(const ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_payload_LENGTH>& rhs) { button2_mqtt_payload_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_payload_LENGTH>& get_button2_mqtt_payload() const { return button2_mqtt_payload_; }
    inline const char* button2_mqtt_payload() const { return button2_mqtt_payload_.get_const(); }

    static constexpr char const* BUTTON1_DETACHED_LIGHT_ID_NAME = "button1_detached_light_id";
    inline void clear_button1_detached_light_id() { button1_detached_light_id_.clear(); }
    inline void set_button1_detached_light_id(const int32_t& value) { button1_detached_light_id_ = value; }
    inline void set_button1_detached_light_id(const int32_t&& value) { button1_detached_light_id_ = value; }
    inline int32_t& mutable_button1_detached_light_id() { return button1_detached_light_id_.get(); }
    inline const int32_t& get_button1_detached_light_id() const { return button1_detached_light_id_.get(); }
    inline int32_t button1_detached_light_id() const { return button1_detached_light_id_.get(); }

    static constexpr char const* BUTTON2_DETACHED_LIGHT_ID_NAME = "button2_detached_light_id";
    inline void clear_button2_detached_light_id() { button2_detached_light_id_.clear(); }
    inline void set_button2_detached_light_id(const int32_t& value) { button2_detached_light_id_ = value; }
    inline void set_button2_detached_light_id(const int32_t&& value) { button2_detached_light_id_ = value; }
    inline int32_t& mutable_button2_detached_light_id() { return button2_detached_light_id_.get(); }
    inline const int32_t& get_button2_detached_light_id() const { return button2_detached_light_id_.get(); }
    inline int32_t button2_detached_light_id() const { return button2_detached_light_id_.get(); }

    static constexpr char const* GLOBAL_SCENES_NAME = "global_scenes";
    inline const NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>& global_scenes(uint32_t index) const { return global_scenes_[index]; }
    inline void clear_global_scenes() { global_scenes_.clear(); }
    inline void set_global_scenes(uint32_t index, const NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>& value) { global_scenes_.set(index, value); }
    inline void set_global_scenes(uint32_t index, const NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>&& value) { global_scenes_.set(index, value); }
    inline void set_global_scenes(const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>, NSPanelConfig_global_scenes_REP_LENGTH>& values) { global_scenes_ = values; }
    inline void add_global_scenes(const NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>& value) { global_scenes_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>, NSPanelConfig_global_scenes_REP_LENGTH>& mutable_global_scenes() { return global_scenes_; }
    inline NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>& mutable_global_scenes(uint32_t index) { return global_scenes_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>, NSPanelConfig_global_scenes_REP_LENGTH>& get_global_scenes() const { return global_scenes_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>, NSPanelConfig_global_scenes_REP_LENGTH>& global_scenes() const { return global_scenes_; }

    static constexpr char const* OPTIMISTIC_MODE_NAME = "optimistic_mode";
    inline void clear_optimistic_mode() { optimistic_mode_.clear(); }
    inline void set_optimistic_mode(const bool& value) { optimistic_mode_ = value; }
    inline void set_optimistic_mode(const bool&& value) { optimistic_mode_ = value; }
    inline bool& mutable_optimistic_mode() { return optimistic_mode_.get(); }
    inline const bool& get_optimistic_mode() const { return optimistic_mode_.get(); }
    inline bool optimistic_mode() const { return optimistic_mode_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = name_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAME), buffer, false);
      }

      if((0 != default_room_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = default_room_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DEFAULT_ROOM), buffer, false);
      }

      if((0 != default_page_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = default_page_.serialize_with_id(static_cast<uint32_t>(FieldNumber::DEFAULT_PAGE), buffer, false);
      }

      if((0 != screensaver_activation_timeout_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = screensaver_activation_timeout_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SCREENSAVER_ACTIVATION_TIMEOUT), buffer, false);
      }

      if((0 != min_button_push_time_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = min_button_push_time_.serialize_with_id(static_cast<uint32_t>(FieldNumber::MIN_BUTTON_PUSH_TIME), buffer, false);
      }

      if((0 != button_long_press_time_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = button_long_press_time_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON_LONG_PRESS_TIME), buffer, false);
      }

      if((0 != special_mode_trigger_time_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = special_mode_trigger_time_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SPECIAL_MODE_TRIGGER_TIME), buffer, false);
      }

      if((0 != special_mode_release_time_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = special_mode_release_time_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SPECIAL_MODE_RELEASE_TIME), buffer, false);
      }

      if((0 != screen_dim_level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = screen_dim_level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SCREEN_DIM_LEVEL), buffer, false);
      }

      if((0 != screensaver_dim_level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = screensaver_dim_level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SCREENSAVER_DIM_LEVEL), buffer, false);
      }

      if((static_cast<NSPanelScreensaverMode>(0) != screensaver_mode_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = screensaver_mode_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SCREENSAVER_MODE), buffer, false);
      }

      if((false != clock_us_style_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = clock_us_style_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CLOCK_US_STYLE), buffer, false);
      }

      if((false != use_fahrenheit_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = use_fahrenheit_.serialize_with_id(static_cast<uint32_t>(FieldNumber::USE_FAHRENHEIT), buffer, false);
      }

      if((false != is_us_panel_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = is_us_panel_.serialize_with_id(static_cast<uint32_t>(FieldNumber::IS_US_PANEL), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = room_ids_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ROOM_IDS), buffer, false);
      }

      if((false != reverse_relays_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = reverse_relays_.serialize_with_id(static_cast<uint32_t>(FieldNumber::REVERSE_RELAYS), buffer, false);
      }

      if((false != relay1_default_mode_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = relay1_default_mode_.serialize_with_id(static_cast<uint32_t>(FieldNumber::RELAY1_DEFAULT_MODE), buffer, false);
      }

      if((false != relay2_default_mode_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = relay2_default_mode_.serialize_with_id(static_cast<uint32_t>(FieldNumber::RELAY2_DEFAULT_MODE), buffer, false);
      }

      if((0.0 != temperature_calibration_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = temperature_calibration_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TEMPERATURE_CALIBRATION), buffer, false);
      }

      if((0 != button1_mode_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = button1_mode_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON1_MODE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = button1_mqtt_topic_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON1_MQTT_TOPIC), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = button1_mqtt_payload_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON1_MQTT_PAYLOAD), buffer, false);
      }

      if((0 != button2_mode_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = button2_mode_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON2_MODE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = button2_mqtt_topic_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON2_MQTT_TOPIC), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = button2_mqtt_payload_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON2_MQTT_PAYLOAD), buffer, false);
      }

      if((0 != button1_detached_light_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = button1_detached_light_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON1_DETACHED_LIGHT_ID), buffer, false);
      }

      if((0 != button2_detached_light_id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = button2_detached_light_id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::BUTTON2_DETACHED_LIGHT_ID), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = global_scenes_.serialize_with_id(static_cast<uint32_t>(FieldNumber::GLOBAL_SCENES), buffer, false);
      }

      if((false != optimistic_mode_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = optimistic_mode_.serialize_with_id(static_cast<uint32_t>(FieldNumber::OPTIMISTIC_MODE), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::NAME:
            return_value = name_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::DEFAULT_ROOM:
            return_value = default_room_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::DEFAULT_PAGE:
            return_value = default_page_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SCREENSAVER_ACTIVATION_TIMEOUT:
            return_value = screensaver_activation_timeout_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::MIN_BUTTON_PUSH_TIME:
            return_value = min_button_push_time_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON_LONG_PRESS_TIME:
            return_value = button_long_press_time_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SPECIAL_MODE_TRIGGER_TIME:
            return_value = special_mode_trigger_time_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SPECIAL_MODE_RELEASE_TIME:
            return_value = special_mode_release_time_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SCREEN_DIM_LEVEL:
            return_value = screen_dim_level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SCREENSAVER_DIM_LEVEL:
            return_value = screensaver_dim_level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SCREENSAVER_MODE:
            return_value = screensaver_mode_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CLOCK_US_STYLE:
            return_value = clock_us_style_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::USE_FAHRENHEIT:
            return_value = use_fahrenheit_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::IS_US_PANEL:
            return_value = is_us_panel_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::ROOM_IDS:
            return_value = room_ids_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::REVERSE_RELAYS:
            return_value = reverse_relays_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::RELAY1_DEFAULT_MODE:
            return_value = relay1_default_mode_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::RELAY2_DEFAULT_MODE:
            return_value = relay2_default_mode_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TEMPERATURE_CALIBRATION:
            return_value = temperature_calibration_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON1_MODE:
            return_value = button1_mode_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON1_MQTT_TOPIC:
            return_value = button1_mqtt_topic_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON1_MQTT_PAYLOAD:
            return_value = button1_mqtt_payload_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON2_MODE:
            return_value = button2_mode_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON2_MQTT_TOPIC:
            return_value = button2_mqtt_topic_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON2_MQTT_PAYLOAD:
            return_value = button2_mqtt_payload_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON1_DETACHED_LIGHT_ID:
            return_value = button1_detached_light_id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::BUTTON2_DETACHED_LIGHT_ID:
            return_value = button2_detached_light_id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::GLOBAL_SCENES:
            return_value = global_scenes_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::OPTIMISTIC_MODE:
            return_value = optimistic_mode_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_name();
      clear_default_room();
      clear_default_page();
      clear_screensaver_activation_timeout();
      clear_min_button_push_time();
      clear_button_long_press_time();
      clear_special_mode_trigger_time();
      clear_special_mode_release_time();
      clear_screen_dim_level();
      clear_screensaver_dim_level();
      clear_screensaver_mode();
      clear_clock_us_style();
      clear_use_fahrenheit();
      clear_is_us_panel();
      clear_room_ids();
      clear_reverse_relays();
      clear_relay1_default_mode();
      clear_relay2_default_mode();
      clear_temperature_calibration();
      clear_button1_mode();
      clear_button1_mqtt_topic();
      clear_button1_mqtt_payload();
      clear_button2_mode();
      clear_button2_mqtt_topic();
      clear_button2_mqtt_payload();
      clear_button1_detached_light_id();
      clear_button2_detached_light_id();
      clear_global_scenes();
      clear_optimistic_mode();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::NAME:
          name = NAME_NAME;
          break;
        case FieldNumber::DEFAULT_ROOM:
          name = DEFAULT_ROOM_NAME;
          break;
        case FieldNumber::DEFAULT_PAGE:
          name = DEFAULT_PAGE_NAME;
          break;
        case FieldNumber::SCREENSAVER_ACTIVATION_TIMEOUT:
          name = SCREENSAVER_ACTIVATION_TIMEOUT_NAME;
          break;
        case FieldNumber::MIN_BUTTON_PUSH_TIME:
          name = MIN_BUTTON_PUSH_TIME_NAME;
          break;
        case FieldNumber::BUTTON_LONG_PRESS_TIME:
          name = BUTTON_LONG_PRESS_TIME_NAME;
          break;
        case FieldNumber::SPECIAL_MODE_TRIGGER_TIME:
          name = SPECIAL_MODE_TRIGGER_TIME_NAME;
          break;
        case FieldNumber::SPECIAL_MODE_RELEASE_TIME:
          name = SPECIAL_MODE_RELEASE_TIME_NAME;
          break;
        case FieldNumber::SCREEN_DIM_LEVEL:
          name = SCREEN_DIM_LEVEL_NAME;
          break;
        case FieldNumber::SCREENSAVER_DIM_LEVEL:
          name = SCREENSAVER_DIM_LEVEL_NAME;
          break;
        case FieldNumber::SCREENSAVER_MODE:
          name = SCREENSAVER_MODE_NAME;
          break;
        case FieldNumber::CLOCK_US_STYLE:
          name = CLOCK_US_STYLE_NAME;
          break;
        case FieldNumber::USE_FAHRENHEIT:
          name = USE_FAHRENHEIT_NAME;
          break;
        case FieldNumber::IS_US_PANEL:
          name = IS_US_PANEL_NAME;
          break;
        case FieldNumber::ROOM_IDS:
          name = ROOM_IDS_NAME;
          break;
        case FieldNumber::REVERSE_RELAYS:
          name = REVERSE_RELAYS_NAME;
          break;
        case FieldNumber::RELAY1_DEFAULT_MODE:
          name = RELAY1_DEFAULT_MODE_NAME;
          break;
        case FieldNumber::RELAY2_DEFAULT_MODE:
          name = RELAY2_DEFAULT_MODE_NAME;
          break;
        case FieldNumber::TEMPERATURE_CALIBRATION:
          name = TEMPERATURE_CALIBRATION_NAME;
          break;
        case FieldNumber::BUTTON1_MODE:
          name = BUTTON1_MODE_NAME;
          break;
        case FieldNumber::BUTTON1_MQTT_TOPIC:
          name = BUTTON1_MQTT_TOPIC_NAME;
          break;
        case FieldNumber::BUTTON1_MQTT_PAYLOAD:
          name = BUTTON1_MQTT_PAYLOAD_NAME;
          break;
        case FieldNumber::BUTTON2_MODE:
          name = BUTTON2_MODE_NAME;
          break;
        case FieldNumber::BUTTON2_MQTT_TOPIC:
          name = BUTTON2_MQTT_TOPIC_NAME;
          break;
        case FieldNumber::BUTTON2_MQTT_PAYLOAD:
          name = BUTTON2_MQTT_PAYLOAD_NAME;
          break;
        case FieldNumber::BUTTON1_DETACHED_LIGHT_ID:
          name = BUTTON1_DETACHED_LIGHT_ID_NAME;
          break;
        case FieldNumber::BUTTON2_DETACHED_LIGHT_ID:
          name = BUTTON2_DETACHED_LIGHT_ID_NAME;
          break;
        case FieldNumber::GLOBAL_SCENES:
          name = GLOBAL_SCENES_NAME;
          break;
        case FieldNumber::OPTIMISTIC_MODE:
          name = OPTIMISTIC_MODE_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = name_.to_string(left_chars, indent_level + 2, NAME_NAME, true);
      left_chars = default_room_.to_string(left_chars, indent_level + 2, DEFAULT_ROOM_NAME, false);
      left_chars = default_page_.to_string(left_chars, indent_level + 2, DEFAULT_PAGE_NAME, false);
      left_chars = screensaver_activation_timeout_.to_string(left_chars, indent_level + 2, SCREENSAVER_ACTIVATION_TIMEOUT_NAME, false);
      left_chars = min_button_push_time_.to_string(left_chars, indent_level + 2, MIN_BUTTON_PUSH_TIME_NAME, false);
      left_chars = button_long_press_time_.to_string(left_chars, indent_level + 2, BUTTON_LONG_PRESS_TIME_NAME, false);
      left_chars = special_mode_trigger_time_.to_string(left_chars, indent_level + 2, SPECIAL_MODE_TRIGGER_TIME_NAME, false);
      left_chars = special_mode_release_time_.to_string(left_chars, indent_level + 2, SPECIAL_MODE_RELEASE_TIME_NAME, false);
      left_chars = screen_dim_level_.to_string(left_chars, indent_level + 2, SCREEN_DIM_LEVEL_NAME, false);
      left_chars = screensaver_dim_level_.to_string(left_chars, indent_level + 2, SCREENSAVER_DIM_LEVEL_NAME, false);
      left_chars = screensaver_mode_.to_string(left_chars, indent_level + 2, SCREENSAVER_MODE_NAME, false);
      left_chars = clock_us_style_.to_string(left_chars, indent_level + 2, CLOCK_US_STYLE_NAME, false);
      left_chars = use_fahrenheit_.to_string(left_chars, indent_level + 2, USE_FAHRENHEIT_NAME, false);
      left_chars = is_us_panel_.to_string(left_chars, indent_level + 2, IS_US_PANEL_NAME, false);
      left_chars = room_ids_.to_string(left_chars, indent_level + 2, ROOM_IDS_NAME, false);
      left_chars = reverse_relays_.to_string(left_chars, indent_level + 2, REVERSE_RELAYS_NAME, false);
      left_chars = relay1_default_mode_.to_string(left_chars, indent_level + 2, RELAY1_DEFAULT_MODE_NAME, false);
      left_chars = relay2_default_mode_.to_string(left_chars, indent_level + 2, RELAY2_DEFAULT_MODE_NAME, false);
      left_chars = temperature_calibration_.to_string(left_chars, indent_level + 2, TEMPERATURE_CALIBRATION_NAME, false);
      left_chars = button1_mode_.to_string(left_chars, indent_level + 2, BUTTON1_MODE_NAME, false);
      left_chars = button1_mqtt_topic_.to_string(left_chars, indent_level + 2, BUTTON1_MQTT_TOPIC_NAME, false);
      left_chars = button1_mqtt_payload_.to_string(left_chars, indent_level + 2, BUTTON1_MQTT_PAYLOAD_NAME, false);
      left_chars = button2_mode_.to_string(left_chars, indent_level + 2, BUTTON2_MODE_NAME, false);
      left_chars = button2_mqtt_topic_.to_string(left_chars, indent_level + 2, BUTTON2_MQTT_TOPIC_NAME, false);
      left_chars = button2_mqtt_payload_.to_string(left_chars, indent_level + 2, BUTTON2_MQTT_PAYLOAD_NAME, false);
      left_chars = button1_detached_light_id_.to_string(left_chars, indent_level + 2, BUTTON1_DETACHED_LIGHT_ID_NAME, false);
      left_chars = button2_detached_light_id_.to_string(left_chars, indent_level + 2, BUTTON2_DETACHED_LIGHT_ID_NAME, false);
      left_chars = global_scenes_.to_string(left_chars, indent_level + 2, GLOBAL_SCENES_NAME, false);
      left_chars = optimistic_mode_.to_string(left_chars, indent_level + 2, OPTIMISTIC_MODE_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      ::EmbeddedProto::FieldString<NSPanelConfig_name_LENGTH> name_;
      EmbeddedProto::int32 default_room_ = 0;
      EmbeddedProto::int32 default_page_ = 0;
      EmbeddedProto::int32 screensaver_activation_timeout_ = 0;
      EmbeddedProto::int32 min_button_push_time_ = 0;
      EmbeddedProto::int32 button_long_press_time_ = 0;
      EmbeddedProto::int32 special_mode_trigger_time_ = 0;
      EmbeddedProto::int32 special_mode_release_time_ = 0;
      EmbeddedProto::int32 screen_dim_level_ = 0;
      EmbeddedProto::int32 screensaver_dim_level_ = 0;
      EmbeddedProto::enumeration<NSPanelScreensaverMode> screensaver_mode_ = static_cast<NSPanelScreensaverMode>(0);
      EmbeddedProto::boolean clock_us_style_ = false;
      EmbeddedProto::boolean use_fahrenheit_ = false;
      EmbeddedProto::boolean is_us_panel_ = false;
      ::EmbeddedProto::RepeatedFieldFixedSize<EmbeddedProto::int32, NSPanelConfig_room_ids_REP_LENGTH> room_ids_;
      EmbeddedProto::boolean reverse_relays_ = false;
      EmbeddedProto::boolean relay1_default_mode_ = false;
      EmbeddedProto::boolean relay2_default_mode_ = false;
      EmbeddedProto::floatfixed temperature_calibration_ = 0.0;
      EmbeddedProto::int32 button1_mode_ = 0;
      ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_topic_LENGTH> button1_mqtt_topic_;
      ::EmbeddedProto::FieldString<NSPanelConfig_button1_mqtt_payload_LENGTH> button1_mqtt_payload_;
      EmbeddedProto::int32 button2_mode_ = 0;
      ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_topic_LENGTH> button2_mqtt_topic_;
      ::EmbeddedProto::FieldString<NSPanelConfig_button2_mqtt_payload_LENGTH> button2_mqtt_payload_;
      EmbeddedProto::int32 button1_detached_light_id_ = 0;
      EmbeddedProto::int32 button2_detached_light_id_ = 0;
      ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelConfig_global_scenes_NSPanelScene_name_LENGTH>, NSPanelConfig_global_scenes_REP_LENGTH> global_scenes_;
      EmbeddedProto::boolean optimistic_mode_ = false;

};

template<
    uint32_t NSPanelRoomStatus_name_LENGTH, 
    uint32_t NSPanelRoomStatus_lights_REP_LENGTH, 
    uint32_t NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH, 
    uint32_t NSPanelRoomStatus_scenes_REP_LENGTH, 
    uint32_t NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH
>
class NSPanelRoomStatus final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelRoomStatus() = default;
    NSPanelRoomStatus(const NSPanelRoomStatus& rhs )
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_average_dim_level(rhs.get_average_dim_level());
      set_ceiling_lights_dim_level(rhs.get_ceiling_lights_dim_level());
      set_table_lights_dim_level(rhs.get_table_lights_dim_level());
      set_average_color_temperature(rhs.get_average_color_temperature());
      set_ceiling_lights_color_temperature_value(rhs.get_ceiling_lights_color_temperature_value());
      set_table_lights_color_temperature_value(rhs.get_table_lights_color_temperature_value());
      set_lights(rhs.get_lights());
      set_scenes(rhs.get_scenes());
      set_has_ceiling_lights(rhs.get_has_ceiling_lights());
      set_has_table_lights(rhs.get_has_table_lights());
    }

    NSPanelRoomStatus(const NSPanelRoomStatus&& rhs ) noexcept
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_average_dim_level(rhs.get_average_dim_level());
      set_ceiling_lights_dim_level(rhs.get_ceiling_lights_dim_level());
      set_table_lights_dim_level(rhs.get_table_lights_dim_level());
      set_average_color_temperature(rhs.get_average_color_temperature());
      set_ceiling_lights_color_temperature_value(rhs.get_ceiling_lights_color_temperature_value());
      set_table_lights_color_temperature_value(rhs.get_table_lights_color_temperature_value());
      set_lights(rhs.get_lights());
      set_scenes(rhs.get_scenes());
      set_has_ceiling_lights(rhs.get_has_ceiling_lights());
      set_has_table_lights(rhs.get_has_table_lights());
    }

    ~NSPanelRoomStatus() override = default;

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      ID = 1,
      NAME = 2,
      AVERAGE_DIM_LEVEL = 3,
      CEILING_LIGHTS_DIM_LEVEL = 4,
      TABLE_LIGHTS_DIM_LEVEL = 5,
      AVERAGE_COLOR_TEMPERATURE = 6,
      CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE = 7,
      TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE = 8,
      LIGHTS = 9,
      SCENES = 10,
      HAS_CEILING_LIGHTS = 11,
      HAS_TABLE_LIGHTS = 12
    };

    NSPanelRoomStatus& operator=(const NSPanelRoomStatus& rhs)
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_average_dim_level(rhs.get_average_dim_level());
      set_ceiling_lights_dim_level(rhs.get_ceiling_lights_dim_level());
      set_table_lights_dim_level(rhs.get_table_lights_dim_level());
      set_average_color_temperature(rhs.get_average_color_temperature());
      set_ceiling_lights_color_temperature_value(rhs.get_ceiling_lights_color_temperature_value());
      set_table_lights_color_temperature_value(rhs.get_table_lights_color_temperature_value());
      set_lights(rhs.get_lights());
      set_scenes(rhs.get_scenes());
      set_has_ceiling_lights(rhs.get_has_ceiling_lights());
      set_has_table_lights(rhs.get_has_table_lights());
      return *this;
    }

    NSPanelRoomStatus& operator=(const NSPanelRoomStatus&& rhs) noexcept
    {
      set_id(rhs.get_id());
      set_name(rhs.get_name());
      set_average_dim_level(rhs.get_average_dim_level());
      set_ceiling_lights_dim_level(rhs.get_ceiling_lights_dim_level());
      set_table_lights_dim_level(rhs.get_table_lights_dim_level());
      set_average_color_temperature(rhs.get_average_color_temperature());
      set_ceiling_lights_color_temperature_value(rhs.get_ceiling_lights_color_temperature_value());
      set_table_lights_color_temperature_value(rhs.get_table_lights_color_temperature_value());
      set_lights(rhs.get_lights());
      set_scenes(rhs.get_scenes());
      set_has_ceiling_lights(rhs.get_has_ceiling_lights());
      set_has_table_lights(rhs.get_has_table_lights());
      return *this;
    }

    static constexpr char const* ID_NAME = "id";
    inline void clear_id() { id_.clear(); }
    inline void set_id(const int32_t& value) { id_ = value; }
    inline void set_id(const int32_t&& value) { id_ = value; }
    inline int32_t& mutable_id() { return id_.get(); }
    inline const int32_t& get_id() const { return id_.get(); }
    inline int32_t id() const { return id_.get(); }

    static constexpr char const* NAME_NAME = "name";
    inline void clear_name() { name_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelRoomStatus_name_LENGTH>& mutable_name() { return name_; }
    inline void set_name(const ::EmbeddedProto::FieldString<NSPanelRoomStatus_name_LENGTH>& rhs) { name_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelRoomStatus_name_LENGTH>& get_name() const { return name_; }
    inline const char* name() const { return name_.get_const(); }

    static constexpr char const* AVERAGE_DIM_LEVEL_NAME = "average_dim_level";
    inline void clear_average_dim_level() { average_dim_level_.clear(); }
    inline void set_average_dim_level(const int32_t& value) { average_dim_level_ = value; }
    inline void set_average_dim_level(const int32_t&& value) { average_dim_level_ = value; }
    inline int32_t& mutable_average_dim_level() { return average_dim_level_.get(); }
    inline const int32_t& get_average_dim_level() const { return average_dim_level_.get(); }
    inline int32_t average_dim_level() const { return average_dim_level_.get(); }

    static constexpr char const* CEILING_LIGHTS_DIM_LEVEL_NAME = "ceiling_lights_dim_level";
    inline void clear_ceiling_lights_dim_level() { ceiling_lights_dim_level_.clear(); }
    inline void set_ceiling_lights_dim_level(const int32_t& value) { ceiling_lights_dim_level_ = value; }
    inline void set_ceiling_lights_dim_level(const int32_t&& value) { ceiling_lights_dim_level_ = value; }
    inline int32_t& mutable_ceiling_lights_dim_level() { return ceiling_lights_dim_level_.get(); }
    inline const int32_t& get_ceiling_lights_dim_level() const { return ceiling_lights_dim_level_.get(); }
    inline int32_t ceiling_lights_dim_level() const { return ceiling_lights_dim_level_.get(); }

    static constexpr char const* TABLE_LIGHTS_DIM_LEVEL_NAME = "table_lights_dim_level";
    inline void clear_table_lights_dim_level() { table_lights_dim_level_.clear(); }
    inline void set_table_lights_dim_level(const int32_t& value) { table_lights_dim_level_ = value; }
    inline void set_table_lights_dim_level(const int32_t&& value) { table_lights_dim_level_ = value; }
    inline int32_t& mutable_table_lights_dim_level() { return table_lights_dim_level_.get(); }
    inline const int32_t& get_table_lights_dim_level() const { return table_lights_dim_level_.get(); }
    inline int32_t table_lights_dim_level() const { return table_lights_dim_level_.get(); }

    static constexpr char const* AVERAGE_COLOR_TEMPERATURE_NAME = "average_color_temperature";
    inline void clear_average_color_temperature() { average_color_temperature_.clear(); }
    inline void set_average_color_temperature(const int32_t& value) { average_color_temperature_ = value; }
    inline void set_average_color_temperature(const int32_t&& value) { average_color_temperature_ = value; }
    inline int32_t& mutable_average_color_temperature() { return average_color_temperature_.get(); }
    inline const int32_t& get_average_color_temperature() const { return average_color_temperature_.get(); }
    inline int32_t average_color_temperature() const { return average_color_temperature_.get(); }

    static constexpr char const* CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE_NAME = "ceiling_lights_color_temperature_value";
    inline void clear_ceiling_lights_color_temperature_value() { ceiling_lights_color_temperature_value_.clear(); }
    inline void set_ceiling_lights_color_temperature_value(const int32_t& value) { ceiling_lights_color_temperature_value_ = value; }
    inline void set_ceiling_lights_color_temperature_value(const int32_t&& value) { ceiling_lights_color_temperature_value_ = value; }
    inline int32_t& mutable_ceiling_lights_color_temperature_value() { return ceiling_lights_color_temperature_value_.get(); }
    inline const int32_t& get_ceiling_lights_color_temperature_value() const { return ceiling_lights_color_temperature_value_.get(); }
    inline int32_t ceiling_lights_color_temperature_value() const { return ceiling_lights_color_temperature_value_.get(); }

    static constexpr char const* TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE_NAME = "table_lights_color_temperature_value";
    inline void clear_table_lights_color_temperature_value() { table_lights_color_temperature_value_.clear(); }
    inline void set_table_lights_color_temperature_value(const int32_t& value) { table_lights_color_temperature_value_ = value; }
    inline void set_table_lights_color_temperature_value(const int32_t&& value) { table_lights_color_temperature_value_ = value; }
    inline int32_t& mutable_table_lights_color_temperature_value() { return table_lights_color_temperature_value_.get(); }
    inline const int32_t& get_table_lights_color_temperature_value() const { return table_lights_color_temperature_value_.get(); }
    inline int32_t table_lights_color_temperature_value() const { return table_lights_color_temperature_value_.get(); }

    static constexpr char const* LIGHTS_NAME = "lights";
    inline const NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>& lights(uint32_t index) const { return lights_[index]; }
    inline void clear_lights() { lights_.clear(); }
    inline void set_lights(uint32_t index, const NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>& value) { lights_.set(index, value); }
    inline void set_lights(uint32_t index, const NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>&& value) { lights_.set(index, value); }
    inline void set_lights(const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>, NSPanelRoomStatus_lights_REP_LENGTH>& values) { lights_ = values; }
    inline void add_lights(const NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>& value) { lights_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>, NSPanelRoomStatus_lights_REP_LENGTH>& mutable_lights() { return lights_; }
    inline NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>& mutable_lights(uint32_t index) { return lights_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>, NSPanelRoomStatus_lights_REP_LENGTH>& get_lights() const { return lights_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>, NSPanelRoomStatus_lights_REP_LENGTH>& lights() const { return lights_; }

    static constexpr char const* SCENES_NAME = "scenes";
    inline const NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>& scenes(uint32_t index) const { return scenes_[index]; }
    inline void clear_scenes() { scenes_.clear(); }
    inline void set_scenes(uint32_t index, const NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>& value) { scenes_.set(index, value); }
    inline void set_scenes(uint32_t index, const NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>&& value) { scenes_.set(index, value); }
    inline void set_scenes(const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>, NSPanelRoomStatus_scenes_REP_LENGTH>& values) { scenes_ = values; }
    inline void add_scenes(const NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>& value) { scenes_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>, NSPanelRoomStatus_scenes_REP_LENGTH>& mutable_scenes() { return scenes_; }
    inline NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>& mutable_scenes(uint32_t index) { return scenes_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>, NSPanelRoomStatus_scenes_REP_LENGTH>& get_scenes() const { return scenes_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>, NSPanelRoomStatus_scenes_REP_LENGTH>& scenes() const { return scenes_; }

    static constexpr char const* HAS_CEILING_LIGHTS_NAME = "has_ceiling_lights";
    inline void clear_has_ceiling_lights() { has_ceiling_lights_.clear(); }
    inline void set_has_ceiling_lights(const bool& value) { has_ceiling_lights_ = value; }
    inline void set_has_ceiling_lights(const bool&& value) { has_ceiling_lights_ = value; }
    inline bool& mutable_has_ceiling_lights() { return has_ceiling_lights_.get(); }
    inline const bool& get_has_ceiling_lights() const { return has_ceiling_lights_.get(); }
    inline bool has_ceiling_lights() const { return has_ceiling_lights_.get(); }

    static constexpr char const* HAS_TABLE_LIGHTS_NAME = "has_table_lights";
    inline void clear_has_table_lights() { has_table_lights_.clear(); }
    inline void set_has_table_lights(const bool& value) { has_table_lights_ = value; }
    inline void set_has_table_lights(const bool&& value) { has_table_lights_ = value; }
    inline bool& mutable_has_table_lights() { return has_table_lights_.get(); }
    inline const bool& get_has_table_lights() const { return has_table_lights_.get(); }
    inline bool has_table_lights() const { return has_table_lights_.get(); }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((0 != id_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = id_.serialize_with_id(static_cast<uint32_t>(FieldNumber::ID), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = name_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NAME), buffer, false);
      }

      if((0 != average_dim_level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = average_dim_level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::AVERAGE_DIM_LEVEL), buffer, false);
      }

      if((0 != ceiling_lights_dim_level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = ceiling_lights_dim_level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CEILING_LIGHTS_DIM_LEVEL), buffer, false);
      }

      if((0 != table_lights_dim_level_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = table_lights_dim_level_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TABLE_LIGHTS_DIM_LEVEL), buffer, false);
      }

      if((0 != average_color_temperature_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = average_color_temperature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::AVERAGE_COLOR_TEMPERATURE), buffer, false);
      }

      if((0 != ceiling_lights_color_temperature_value_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = ceiling_lights_color_temperature_value_.serialize_with_id(static_cast<uint32_t>(FieldNumber::CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE), buffer, false);
      }

      if((0 != table_lights_color_temperature_value_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = table_lights_color_temperature_value_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = lights_.serialize_with_id(static_cast<uint32_t>(FieldNumber::LIGHTS), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = scenes_.serialize_with_id(static_cast<uint32_t>(FieldNumber::SCENES), buffer, false);
      }

      if((false != has_ceiling_lights_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = has_ceiling_lights_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_CEILING_LIGHTS), buffer, false);
      }

      if((false != has_table_lights_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = has_table_lights_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HAS_TABLE_LIGHTS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::ID:
            return_value = id_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NAME:
            return_value = name_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::AVERAGE_DIM_LEVEL:
            return_value = average_dim_level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CEILING_LIGHTS_DIM_LEVEL:
            return_value = ceiling_lights_dim_level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TABLE_LIGHTS_DIM_LEVEL:
            return_value = table_lights_dim_level_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::AVERAGE_COLOR_TEMPERATURE:
            return_value = average_color_temperature_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE:
            return_value = ceiling_lights_color_temperature_value_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE:
            return_value = table_lights_color_temperature_value_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::LIGHTS:
            return_value = lights_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::SCENES:
            return_value = scenes_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::HAS_CEILING_LIGHTS:
            return_value = has_ceiling_lights_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::HAS_TABLE_LIGHTS:
            return_value = has_table_lights_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_id();
      clear_name();
      clear_average_dim_level();
      clear_ceiling_lights_dim_level();
      clear_table_lights_dim_level();
      clear_average_color_temperature();
      clear_ceiling_lights_color_temperature_value();
      clear_table_lights_color_temperature_value();
      clear_lights();
      clear_scenes();
      clear_has_ceiling_lights();
      clear_has_table_lights();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::ID:
          name = ID_NAME;
          break;
        case FieldNumber::NAME:
          name = NAME_NAME;
          break;
        case FieldNumber::AVERAGE_DIM_LEVEL:
          name = AVERAGE_DIM_LEVEL_NAME;
          break;
        case FieldNumber::CEILING_LIGHTS_DIM_LEVEL:
          name = CEILING_LIGHTS_DIM_LEVEL_NAME;
          break;
        case FieldNumber::TABLE_LIGHTS_DIM_LEVEL:
          name = TABLE_LIGHTS_DIM_LEVEL_NAME;
          break;
        case FieldNumber::AVERAGE_COLOR_TEMPERATURE:
          name = AVERAGE_COLOR_TEMPERATURE_NAME;
          break;
        case FieldNumber::CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE:
          name = CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE_NAME;
          break;
        case FieldNumber::TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE:
          name = TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE_NAME;
          break;
        case FieldNumber::LIGHTS:
          name = LIGHTS_NAME;
          break;
        case FieldNumber::SCENES:
          name = SCENES_NAME;
          break;
        case FieldNumber::HAS_CEILING_LIGHTS:
          name = HAS_CEILING_LIGHTS_NAME;
          break;
        case FieldNumber::HAS_TABLE_LIGHTS:
          name = HAS_TABLE_LIGHTS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = id_.to_string(left_chars, indent_level + 2, ID_NAME, true);
      left_chars = name_.to_string(left_chars, indent_level + 2, NAME_NAME, false);
      left_chars = average_dim_level_.to_string(left_chars, indent_level + 2, AVERAGE_DIM_LEVEL_NAME, false);
      left_chars = ceiling_lights_dim_level_.to_string(left_chars, indent_level + 2, CEILING_LIGHTS_DIM_LEVEL_NAME, false);
      left_chars = table_lights_dim_level_.to_string(left_chars, indent_level + 2, TABLE_LIGHTS_DIM_LEVEL_NAME, false);
      left_chars = average_color_temperature_.to_string(left_chars, indent_level + 2, AVERAGE_COLOR_TEMPERATURE_NAME, false);
      left_chars = ceiling_lights_color_temperature_value_.to_string(left_chars, indent_level + 2, CEILING_LIGHTS_COLOR_TEMPERATURE_VALUE_NAME, false);
      left_chars = table_lights_color_temperature_value_.to_string(left_chars, indent_level + 2, TABLE_LIGHTS_COLOR_TEMPERATURE_VALUE_NAME, false);
      left_chars = lights_.to_string(left_chars, indent_level + 2, LIGHTS_NAME, false);
      left_chars = scenes_.to_string(left_chars, indent_level + 2, SCENES_NAME, false);
      left_chars = has_ceiling_lights_.to_string(left_chars, indent_level + 2, HAS_CEILING_LIGHTS_NAME, false);
      left_chars = has_table_lights_.to_string(left_chars, indent_level + 2, HAS_TABLE_LIGHTS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::int32 id_ = 0;
      ::EmbeddedProto::FieldString<NSPanelRoomStatus_name_LENGTH> name_;
      EmbeddedProto::int32 average_dim_level_ = 0;
      EmbeddedProto::int32 ceiling_lights_dim_level_ = 0;
      EmbeddedProto::int32 table_lights_dim_level_ = 0;
      EmbeddedProto::int32 average_color_temperature_ = 0;
      EmbeddedProto::int32 ceiling_lights_color_temperature_value_ = 0;
      EmbeddedProto::int32 table_lights_color_temperature_value_ = 0;
      ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelLightStatus<NSPanelRoomStatus_lights_NSPanelLightStatus_name_LENGTH>, NSPanelRoomStatus_lights_REP_LENGTH> lights_;
      ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelScene<NSPanelRoomStatus_scenes_NSPanelScene_name_LENGTH>, NSPanelRoomStatus_scenes_REP_LENGTH> scenes_;
      EmbeddedProto::boolean has_ceiling_lights_ = false;
      EmbeddedProto::boolean has_table_lights_ = false;

};

template<
    uint32_t NSPanelStatusReport_mac_address_LENGTH, 
    uint32_t NSPanelStatusReport_temperature_LENGTH, 
    uint32_t NSPanelStatusReport_ip_address_LENGTH, 
    uint32_t NSPanelStatusReport_warnings_REP_LENGTH, 
    uint32_t NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH
>
class NSPanelStatusReport final: public ::EmbeddedProto::MessageInterface
{
  public:
    NSPanelStatusReport() = default;
    NSPanelStatusReport(const NSPanelStatusReport& rhs )
    {
      set_nspanel_state(rhs.get_nspanel_state());
      set_update_progress(rhs.get_update_progress());
      set_rssi(rhs.get_rssi());
      set_heap_used_pct(rhs.get_heap_used_pct());
      set_mac_address(rhs.get_mac_address());
      set_temperature(rhs.get_temperature());
      set_ip_address(rhs.get_ip_address());
      set_warnings(rhs.get_warnings());
    }

    NSPanelStatusReport(const NSPanelStatusReport&& rhs ) noexcept
    {
      set_nspanel_state(rhs.get_nspanel_state());
      set_update_progress(rhs.get_update_progress());
      set_rssi(rhs.get_rssi());
      set_heap_used_pct(rhs.get_heap_used_pct());
      set_mac_address(rhs.get_mac_address());
      set_temperature(rhs.get_temperature());
      set_ip_address(rhs.get_ip_address());
      set_warnings(rhs.get_warnings());
    }

    ~NSPanelStatusReport() override = default;

    enum class state : uint32_t
    {
      ONLINE = 0,
      OFFLINE = 1,
      UPDATING_TFT = 2,
      UPDATING_FIRMWARE = 3,
      UPDATING_LITTLEFS = 4
    };

    enum class FieldNumber : uint32_t
    {
      NOT_SET = 0,
      NSPANEL_STATE = 1,
      UPDATE_PROGRESS = 2,
      RSSI = 3,
      HEAP_USED_PCT = 4,
      MAC_ADDRESS = 5,
      TEMPERATURE = 6,
      IP_ADDRESS = 7,
      WARNINGS = 8
    };

    NSPanelStatusReport& operator=(const NSPanelStatusReport& rhs)
    {
      set_nspanel_state(rhs.get_nspanel_state());
      set_update_progress(rhs.get_update_progress());
      set_rssi(rhs.get_rssi());
      set_heap_used_pct(rhs.get_heap_used_pct());
      set_mac_address(rhs.get_mac_address());
      set_temperature(rhs.get_temperature());
      set_ip_address(rhs.get_ip_address());
      set_warnings(rhs.get_warnings());
      return *this;
    }

    NSPanelStatusReport& operator=(const NSPanelStatusReport&& rhs) noexcept
    {
      set_nspanel_state(rhs.get_nspanel_state());
      set_update_progress(rhs.get_update_progress());
      set_rssi(rhs.get_rssi());
      set_heap_used_pct(rhs.get_heap_used_pct());
      set_mac_address(rhs.get_mac_address());
      set_temperature(rhs.get_temperature());
      set_ip_address(rhs.get_ip_address());
      set_warnings(rhs.get_warnings());
      return *this;
    }

    static constexpr char const* NSPANEL_STATE_NAME = "nspanel_state";
    inline void clear_nspanel_state() { nspanel_state_.clear(); }
    inline void set_nspanel_state(const state& value) { nspanel_state_ = value; }
    inline void set_nspanel_state(const state&& value) { nspanel_state_ = value; }
    inline const state& get_nspanel_state() const { return nspanel_state_.get(); }
    inline state nspanel_state() const { return nspanel_state_.get(); }

    static constexpr char const* UPDATE_PROGRESS_NAME = "update_progress";
    inline void clear_update_progress() { update_progress_.clear(); }
    inline void set_update_progress(const int32_t& value) { update_progress_ = value; }
    inline void set_update_progress(const int32_t&& value) { update_progress_ = value; }
    inline int32_t& mutable_update_progress() { return update_progress_.get(); }
    inline const int32_t& get_update_progress() const { return update_progress_.get(); }
    inline int32_t update_progress() const { return update_progress_.get(); }

    static constexpr char const* RSSI_NAME = "rssi";
    inline void clear_rssi() { rssi_.clear(); }
    inline void set_rssi(const int32_t& value) { rssi_ = value; }
    inline void set_rssi(const int32_t&& value) { rssi_ = value; }
    inline int32_t& mutable_rssi() { return rssi_.get(); }
    inline const int32_t& get_rssi() const { return rssi_.get(); }
    inline int32_t rssi() const { return rssi_.get(); }

    static constexpr char const* HEAP_USED_PCT_NAME = "heap_used_pct";
    inline void clear_heap_used_pct() { heap_used_pct_.clear(); }
    inline void set_heap_used_pct(const int32_t& value) { heap_used_pct_ = value; }
    inline void set_heap_used_pct(const int32_t&& value) { heap_used_pct_ = value; }
    inline int32_t& mutable_heap_used_pct() { return heap_used_pct_.get(); }
    inline const int32_t& get_heap_used_pct() const { return heap_used_pct_.get(); }
    inline int32_t heap_used_pct() const { return heap_used_pct_.get(); }

    static constexpr char const* MAC_ADDRESS_NAME = "mac_address";
    inline void clear_mac_address() { mac_address_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelStatusReport_mac_address_LENGTH>& mutable_mac_address() { return mac_address_; }
    inline void set_mac_address(const ::EmbeddedProto::FieldString<NSPanelStatusReport_mac_address_LENGTH>& rhs) { mac_address_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelStatusReport_mac_address_LENGTH>& get_mac_address() const { return mac_address_; }
    inline const char* mac_address() const { return mac_address_.get_const(); }

    static constexpr char const* TEMPERATURE_NAME = "temperature";
    inline void clear_temperature() { temperature_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelStatusReport_temperature_LENGTH>& mutable_temperature() { return temperature_; }
    inline void set_temperature(const ::EmbeddedProto::FieldString<NSPanelStatusReport_temperature_LENGTH>& rhs) { temperature_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelStatusReport_temperature_LENGTH>& get_temperature() const { return temperature_; }
    inline const char* temperature() const { return temperature_.get_const(); }

    static constexpr char const* IP_ADDRESS_NAME = "ip_address";
    inline void clear_ip_address() { ip_address_.clear(); }
    inline ::EmbeddedProto::FieldString<NSPanelStatusReport_ip_address_LENGTH>& mutable_ip_address() { return ip_address_; }
    inline void set_ip_address(const ::EmbeddedProto::FieldString<NSPanelStatusReport_ip_address_LENGTH>& rhs) { ip_address_.set(rhs); }
    inline const ::EmbeddedProto::FieldString<NSPanelStatusReport_ip_address_LENGTH>& get_ip_address() const { return ip_address_; }
    inline const char* ip_address() const { return ip_address_.get_const(); }

    static constexpr char const* WARNINGS_NAME = "warnings";
    inline const NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>& warnings(uint32_t index) const { return warnings_[index]; }
    inline void clear_warnings() { warnings_.clear(); }
    inline void set_warnings(uint32_t index, const NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>& value) { warnings_.set(index, value); }
    inline void set_warnings(uint32_t index, const NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>&& value) { warnings_.set(index, value); }
    inline void set_warnings(const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>, NSPanelStatusReport_warnings_REP_LENGTH>& values) { warnings_ = values; }
    inline void add_warnings(const NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>& value) { warnings_.add(value); }
    inline ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>, NSPanelStatusReport_warnings_REP_LENGTH>& mutable_warnings() { return warnings_; }
    inline NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>& mutable_warnings(uint32_t index) { return warnings_[index]; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>, NSPanelStatusReport_warnings_REP_LENGTH>& get_warnings() const { return warnings_; }
    inline const ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>, NSPanelStatusReport_warnings_REP_LENGTH>& warnings() const { return warnings_; }


    ::EmbeddedProto::Error serialize(::EmbeddedProto::WriteBufferInterface& buffer) const override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;

      if((static_cast<state>(0) != nspanel_state_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = nspanel_state_.serialize_with_id(static_cast<uint32_t>(FieldNumber::NSPANEL_STATE), buffer, false);
      }

      if((0 != update_progress_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = update_progress_.serialize_with_id(static_cast<uint32_t>(FieldNumber::UPDATE_PROGRESS), buffer, false);
      }

      if((0 != rssi_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = rssi_.serialize_with_id(static_cast<uint32_t>(FieldNumber::RSSI), buffer, false);
      }

      if((0 != heap_used_pct_.get()) && (::EmbeddedProto::Error::NO_ERRORS == return_value))
      {
        return_value = heap_used_pct_.serialize_with_id(static_cast<uint32_t>(FieldNumber::HEAP_USED_PCT), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = mac_address_.serialize_with_id(static_cast<uint32_t>(FieldNumber::MAC_ADDRESS), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = temperature_.serialize_with_id(static_cast<uint32_t>(FieldNumber::TEMPERATURE), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = ip_address_.serialize_with_id(static_cast<uint32_t>(FieldNumber::IP_ADDRESS), buffer, false);
      }

      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = warnings_.serialize_with_id(static_cast<uint32_t>(FieldNumber::WARNINGS), buffer, false);
      }

      return return_value;
    };

    ::EmbeddedProto::Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override
    {
      ::EmbeddedProto::Error return_value = ::EmbeddedProto::Error::NO_ERRORS;
      ::EmbeddedProto::WireFormatter::WireType wire_type = ::EmbeddedProto::WireFormatter::WireType::VARINT;
      uint32_t id_number = 0;
      FieldNumber id_tag = FieldNumber::NOT_SET;

      ::EmbeddedProto::Error tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
      while((::EmbeddedProto::Error::NO_ERRORS == return_value) && (::EmbeddedProto::Error::NO_ERRORS == tag_value))
      {
        id_tag = static_cast<FieldNumber>(id_number);
        switch(id_tag)
        {
          case FieldNumber::NSPANEL_STATE:
            return_value = nspanel_state_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::UPDATE_PROGRESS:
            return_value = update_progress_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::RSSI:
            return_value = rssi_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::HEAP_USED_PCT:
            return_value = heap_used_pct_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::MAC_ADDRESS:
            return_value = mac_address_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::TEMPERATURE:
            return_value = temperature_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::IP_ADDRESS:
            return_value = ip_address_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::WARNINGS:
            return_value = warnings_.deserialize_check_type(buffer, wire_type);
            break;

          case FieldNumber::NOT_SET:
            return_value = ::EmbeddedProto::Error::INVALID_FIELD_ID;
            break;

          default:
            return_value = skip_unknown_field(buffer, wire_type);
            break;
        }

        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          // Read the next tag.
          tag_value = ::EmbeddedProto::WireFormatter::DeserializeTag(buffer, wire_type, id_number);
        }
      }

      // When an error was detect while reading the tag but no other errors where found, set it in the return value.
      if((::EmbeddedProto::Error::NO_ERRORS == return_value)
         && (::EmbeddedProto::Error::NO_ERRORS != tag_value)
         && (::EmbeddedProto::Error::END_OF_BUFFER != tag_value)) // The end of the buffer is not an array in this case.
      {
        return_value = tag_value;
      }

      return return_value;
    };

    void clear() override
    {
      clear_nspanel_state();
      clear_update_progress();
      clear_rssi();
      clear_heap_used_pct();
      clear_mac_address();
      clear_temperature();
      clear_ip_address();
      clear_warnings();

    }

    static char const* field_number_to_name(const FieldNumber fieldNumber)
    {
      char const* name = nullptr;
      switch(fieldNumber)
      {
        case FieldNumber::NSPANEL_STATE:
          name = NSPANEL_STATE_NAME;
          break;
        case FieldNumber::UPDATE_PROGRESS:
          name = UPDATE_PROGRESS_NAME;
          break;
        case FieldNumber::RSSI:
          name = RSSI_NAME;
          break;
        case FieldNumber::HEAP_USED_PCT:
          name = HEAP_USED_PCT_NAME;
          break;
        case FieldNumber::MAC_ADDRESS:
          name = MAC_ADDRESS_NAME;
          break;
        case FieldNumber::TEMPERATURE:
          name = TEMPERATURE_NAME;
          break;
        case FieldNumber::IP_ADDRESS:
          name = IP_ADDRESS_NAME;
          break;
        case FieldNumber::WARNINGS:
          name = WARNINGS_NAME;
          break;
        default:
          name = "Invalid FieldNumber";
          break;
      }
      return name;
    }

#ifdef MSG_TO_STRING

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str) const
    {
      return this->to_string(str, 0, nullptr, true);
    }

    ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const override
    {
      ::EmbeddedProto::string_view left_chars = str;
      int32_t n_chars_used = 0;

      if(!first_field)
      {
        // Add a comma behind the previous field.
        n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }
      }

      if(nullptr != name)
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "\"%s\": {\n", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": {\n", indent_level, " ", name);
        }
      }
      else
      {
        if( 0 == indent_level)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "{\n");
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s{\n", indent_level, " ");
        }
      }
      
      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      left_chars = nspanel_state_.to_string(left_chars, indent_level + 2, NSPANEL_STATE_NAME, true);
      left_chars = update_progress_.to_string(left_chars, indent_level + 2, UPDATE_PROGRESS_NAME, false);
      left_chars = rssi_.to_string(left_chars, indent_level + 2, RSSI_NAME, false);
      left_chars = heap_used_pct_.to_string(left_chars, indent_level + 2, HEAP_USED_PCT_NAME, false);
      left_chars = mac_address_.to_string(left_chars, indent_level + 2, MAC_ADDRESS_NAME, false);
      left_chars = temperature_.to_string(left_chars, indent_level + 2, TEMPERATURE_NAME, false);
      left_chars = ip_address_.to_string(left_chars, indent_level + 2, IP_ADDRESS_NAME, false);
      left_chars = warnings_.to_string(left_chars, indent_level + 2, WARNINGS_NAME, false);
  
      if( 0 == indent_level) 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n}");
      }
      else 
      {
        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s}", indent_level, " ");
      }

      if(0 < n_chars_used)
      {
        left_chars.data += n_chars_used;
        left_chars.size -= n_chars_used;
      }

      return left_chars;
    }

#endif // End of MSG_TO_STRING

  private:


      EmbeddedProto::enumeration<state> nspanel_state_ = static_cast<state>(0);
      EmbeddedProto::int32 update_progress_ = 0;
      EmbeddedProto::int32 rssi_ = 0;
      EmbeddedProto::int32 heap_used_pct_ = 0;
      ::EmbeddedProto::FieldString<NSPanelStatusReport_mac_address_LENGTH> mac_address_;
      ::EmbeddedProto::FieldString<NSPanelStatusReport_temperature_LENGTH> temperature_;
      ::EmbeddedProto::FieldString<NSPanelStatusReport_ip_address_LENGTH> ip_address_;
      ::EmbeddedProto::RepeatedFieldFixedSize<NSPanelWarning<NSPanelStatusReport_warnings_NSPanelWarning_text_LENGTH>, NSPanelStatusReport_warnings_REP_LENGTH> warnings_;

};

#endif // PROTOBUF_NSPANEL_H