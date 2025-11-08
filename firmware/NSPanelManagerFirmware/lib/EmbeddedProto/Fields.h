/*
 *  Copyright (C) 2020-2024 Embedded AMS B.V. - All Rights Reserved
 *
 *  This file is part of Embedded Proto.
 *
 *  Embedded Proto is open source software: you can redistribute it and/or 
 *  modify it under the terms of the GNU General Public License as published 
 *  by the Free Software Foundation, version 3 of the license.
 *
 *  Embedded Proto  is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Embedded Proto. If not, see <https://www.gnu.org/licenses/>.
 *
 *  For commercial and closed source application please visit:
 *  <https://EmbeddedProto.com/license/>.
 *
 *  Embedded AMS B.V.
 *  Info:
 *    info at EmbeddedProto dot com
 *
 *  Postal address:
 *    Atoomweg 2
 *    1627 LE, Hoorn
 *    the Netherlands
 */

#ifndef _FIELDS_H_
#define _FIELDS_H_

#include "Errors.h"
#include "Defines.h"
#include "WireFormatter.h"
#include "WriteBufferInterface.h"
#include "ReadBufferInterface.h"
#include "MessageSizeCalculator.h"

#include <cstdint>

#ifdef MSG_TO_STRING
#include <cstdio>
#endif

namespace EmbeddedProto 
{

  class Field 
  {
    public:
      enum class FieldTypes
      {
        int32,
        int64, 
        uint32, 
        uint64, 
        sint32, 
        sint64, 
        boolean, 
        enumeration,
        fixed64, 
        sfixed64, 
        doublefixed,
        string, 
        bytes, 
        message,
        repeated,
        fixed32, 
        sfixed32, 
        floatfixed
      };


      Field() = default;
      virtual ~Field() = default;

      virtual Error serialize_with_id(uint32_t field_number, WriteBufferInterface& buffer, const bool optional) const = 0;

      virtual Error serialize(WriteBufferInterface& buffer) const = 0;

      //! Deserialize this field from the bytes in the given buffer.
      /*!
        \param buffer data from which the field should be obtained.
        \return NO_ERROR if everything went ok.
      */      
      virtual Error deserialize(ReadBufferInterface& buffer) = 0;

      //! Deserialize this field but also check if given wire type matches the field type.
      /*!
        \param buffer data from which the field should be obtained.
        \param wire_type The wire type obtained from the tag used to match with this field type.
        \return NO_ERROR if everything went ok.
      */
      virtual Error deserialize_check_type(ReadBufferInterface& buffer, 
                                           const ::EmbeddedProto::WireFormatter::WireType& wire_type) = 0;

      //! Calculate the size of this message when serialized.
      /*!
          \return The number of bytes this message will require once serialized.
      */
      uint32_t serialized_size() const;

      //! Reset the field to it's initial value.
      virtual void clear() = 0;

#ifdef MSG_TO_STRING
      //! Write all the data in this field to a human readable string.
      /*!
          \param str A string view object with a pointer to the current location in the character array and its maximum length.
          \param indent_level How many spaces should be added before this field.
          \param name A pointer to the name of this field.
          \param first_field A boolean indicating if this is the first field we serialize in this message.
          \return A string view struct detail how many bytes are left in the string and a pointer to the point where to continue.
      */
      virtual ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const = 0;

#endif // End of MSG_TO_STRING
  };

  template<Field::FieldTypes FIELDTYPE, class VARIABLE_TYPE, WireFormatter::WireType WIRETYPE>
  class FieldTemplate
  {
    public:
      using TYPE = VARIABLE_TYPE;
      using CLASS_TYPE = FieldTemplate<FIELDTYPE, VARIABLE_TYPE, WIRETYPE>;

      FieldTemplate() = default;
      FieldTemplate(const VARIABLE_TYPE& v) : value_(v) { };
      FieldTemplate(const VARIABLE_TYPE&& v) : value_(v) { };
      FieldTemplate(const CLASS_TYPE& ft) : value_(ft.value_) { };

      ~FieldTemplate() = default;

      Error serialize_with_id(uint32_t field_number, WriteBufferInterface& buffer, [[maybe_unused]] const bool optional) const
      {
        Error return_value = WireFormatter::SerializeVarint(WireFormatter::MakeTag(field_number, WIRETYPE), buffer);
        if(Error::NO_ERRORS == return_value)
        {
          return_value = serialize(buffer);
        }
        return return_value;
      }   

      Error serialize(WriteBufferInterface& buffer) const
      {
        return serialize_<FIELDTYPE>(buffer);
      }

      Error deserialize(ReadBufferInterface& buffer)
      {
        return deserialize_<FIELDTYPE>(buffer);
      }

      //! \see Field::deserialize()
      Error deserialize_check_type(ReadBufferInterface& buffer, 
                                   const ::EmbeddedProto::WireFormatter::WireType& wire_type)
      {
        Error return_value = WIRETYPE == wire_type ? Error::NO_ERRORS : Error::INVALID_WIRETYPE;
        if(Error::NO_ERRORS == return_value) 
        {
          return_value = deserialize(buffer);
        }
        return return_value;
      }

      void set(const VARIABLE_TYPE& v) { value_ = v; }      
      void set(const VARIABLE_TYPE&& v) { value_ = v; }

      void set(const CLASS_TYPE& ft) { value_ = ft.value_; }
      void set(const CLASS_TYPE&& ft) { value_ = ft.value_; }
      
      CLASS_TYPE& operator=(const VARIABLE_TYPE& v) 
      { 
        value_ = v;
        return *this;
      }
      CLASS_TYPE& operator=(const VARIABLE_TYPE&& v) 
      { 
        value_ = v;
        return *this;
      }
      CLASS_TYPE& operator=(const CLASS_TYPE& ft)
      { 
        value_ = ft.value_; 
        return *this; 
      }
      CLASS_TYPE& operator=(const CLASS_TYPE&& ft) noexcept
      { 
        value_ = ft.value_;
        return *this;
      }

      const VARIABLE_TYPE& get() const { return value_; }
      VARIABLE_TYPE& get() { return value_; }

      //! This is the conversion operator. 
      /*! 
        Sonar would like this to be explicit but this is not practial in normal usage with other 
        integer and floating point types.
      */
      operator VARIABLE_TYPE() const { return value_; } //NOSONAR

      bool operator==(const VARIABLE_TYPE& rhs) { return value_ == rhs; }
      bool operator!=(const VARIABLE_TYPE& rhs) { return value_ != rhs; }
      bool operator>(const VARIABLE_TYPE& rhs) { return value_ > rhs; }
      bool operator<(const VARIABLE_TYPE& rhs) { return value_ < rhs; }
      bool operator>=(const VARIABLE_TYPE& rhs) { return value_ >= rhs; }
      bool operator<=(const VARIABLE_TYPE& rhs) { return value_ <= rhs; }

      template<Field::FieldTypes FIELDTYPE_RHS, class TYPE_RHS, WireFormatter::WireType WIRETYPE_RHS>
      bool operator==(const FieldTemplate<FIELDTYPE_RHS, TYPE_RHS, WIRETYPE_RHS>& rhs) { return value_ == rhs.get(); }
      template<Field::FieldTypes FIELDTYPE_RHS, class TYPE_RHS, WireFormatter::WireType WIRETYPE_RHS>
      bool operator!=(const FieldTemplate<FIELDTYPE_RHS, TYPE_RHS, WIRETYPE_RHS>& rhs) { return value_ != rhs.get(); }
      template<Field::FieldTypes FIELDTYPE_RHS, class TYPE_RHS, WireFormatter::WireType WIRETYPE_RHS>
      bool operator>(const FieldTemplate<FIELDTYPE_RHS, TYPE_RHS, WIRETYPE_RHS>& rhs) { return value_ > rhs.get(); }
      template<Field::FieldTypes FIELDTYPE_RHS, class TYPE_RHS, WireFormatter::WireType WIRETYPE_RHS>
      bool operator<(const FieldTemplate<FIELDTYPE_RHS, TYPE_RHS, WIRETYPE_RHS>& rhs) { return value_ < rhs.get(); }
      template<Field::FieldTypes FIELDTYPE_RHS, class TYPE_RHS, WireFormatter::WireType WIRETYPE_RHS>
      bool operator>=(const FieldTemplate<FIELDTYPE_RHS, TYPE_RHS, WIRETYPE_RHS>& rhs) { return value_ >= rhs.get(); }
      template<Field::FieldTypes FIELDTYPE_RHS, class TYPE_RHS, WireFormatter::WireType WIRETYPE_RHS>
      bool operator<=(const FieldTemplate<FIELDTYPE_RHS, TYPE_RHS, WIRETYPE_RHS>& rhs) { return value_ <= rhs.get(); }

      void clear() { value_ = static_cast<VARIABLE_TYPE>(0); }

      uint32_t serialized_size() const
      {
        ::EmbeddedProto::MessageSizeCalculator calcBuffer;
        this->serialize(calcBuffer);
        return calcBuffer.get_size();
      }

#ifdef MSG_TO_STRING

      //! Write all the data in this field to a human readable string.
      /*!
          \param str A string view object with a pointer to the current location in the character array and its maximum length.
          \param indent_level How many spaces should be added before this field.
          \param name A pointer to the name of this field.
          \param first_field A boolean indicating if this is the first field we serialize in this message.
          \return A string view struct detail how many bytes are left in the string and a pointer to the point where to continue.
      */
      ::EmbeddedProto::string_view to_string(::EmbeddedProto::string_view& str, const uint32_t indent_level, char const* name, const bool first_field) const
      {
        ::EmbeddedProto::string_view left_chars = str;
        int32_t n_chars_used = 0;

        if(!first_field)
        {
          // Add a comma behind the previous field.
          n_chars_used = snprintf(left_chars.data, left_chars.size, ",\n");
          if(0 < n_chars_used) {
            // Update the character pointer and characters left in the array.
            left_chars.data += n_chars_used;
            left_chars.size -= n_chars_used;
          }
        }

        n_chars_used = 0;

        if(nullptr != name)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": ", indent_level, " ", name);
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s", indent_level, " ");
        }

        if(0 < n_chars_used)
        {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }

        if constexpr((Field::FieldTypes::int32 == FIELDTYPE) ||
                     (Field::FieldTypes::sint32 == FIELDTYPE) ||
                     (Field::FieldTypes::sfixed32 == FIELDTYPE))
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%d", get());
        }
        else if constexpr((Field::FieldTypes::int64 == FIELDTYPE) ||
                          (Field::FieldTypes::sint64 == FIELDTYPE) ||
                          (Field::FieldTypes::sfixed64 == FIELDTYPE))
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%ld", get());
        }
        else if constexpr((Field::FieldTypes::uint32 == FIELDTYPE) ||
                          (Field::FieldTypes::fixed32 == FIELDTYPE))
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%u", get());
        }        
        else if constexpr((Field::FieldTypes::uint64 == FIELDTYPE) ||
                          (Field::FieldTypes::fixed64 == FIELDTYPE))
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%lu", get());
        }
        else if constexpr(Field::FieldTypes::boolean == FIELDTYPE)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%s", 
                                  get() ? "true" : "false");
        }
        else if constexpr(Field::FieldTypes::enumeration == FIELDTYPE)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%d", 
                                  static_cast<uint32_t>(get()));
        }
        else if constexpr(Field::FieldTypes::floatfixed == FIELDTYPE)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%f", get());
        }
        else if constexpr(Field::FieldTypes::doublefixed == FIELDTYPE)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%lf", get());
        }
        else
        {
          // Should never get here.
        }

        if(0 < n_chars_used) {
          // Update the character pointer and characters left in the array.
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }

        return left_chars;
      }

#endif // End of MSG_TO_STRING

    private:

      VARIABLE_TYPE value_;


      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::int32 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(static_cast<uint32_t>(get()), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::int64 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(static_cast<uint64_t>(get()), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::uint32 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::uint64 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sint32 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(WireFormatter::ZigZagEncode(get()), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sint64 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(WireFormatter::ZigZagEncode(get()), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::boolean == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const 
      { 
        const uint8_t byte = get() ? 0x01 : 0x00;
        return buffer.push(byte) ? Error::NO_ERRORS : Error::BUFFER_FULL; 
      }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::enumeration == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeVarint(static_cast<uint32_t>(get()), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::fixed32 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeFixedNoTag(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::fixed64 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerializeFixedNoTag(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sfixed32 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerialzieSFixedNoTag(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sfixed64 == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerialzieSFixedNoTag(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::floatfixed == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerialzieFloatNoTag(get(), buffer); }

      template<Field::FieldTypes SER_FIELDTYPE, typename std::enable_if<Field::FieldTypes::doublefixed == SER_FIELDTYPE, bool>::type = true>
      Error serialize_(WriteBufferInterface& buffer) const { return WireFormatter::SerialzieDoubleNoTag(get(), buffer); }



      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::int32 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeInt(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::int64 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeInt(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::uint32 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeUInt(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::uint64 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeUInt(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sint32 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeSInt(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sint64 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeSInt(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::boolean == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeBool(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::enumeration == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer)
      { 
        uint32_t value = 0;
        const Error return_value = WireFormatter::DeserializeVarint(buffer, value);
        if(::EmbeddedProto::Error::NO_ERRORS == return_value)
        {
          value_ = static_cast<VARIABLE_TYPE>(value);
        }
        return return_value;
      }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::fixed32 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeFixed(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::fixed64 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeFixed(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sfixed32 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeSFixed(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::sfixed64 == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeSFixed(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::floatfixed == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeFloat(buffer, get()); }

      template<Field::FieldTypes DES_FIELDTYPE, typename std::enable_if<Field::FieldTypes::doublefixed == DES_FIELDTYPE, bool>::type = true>
      Error deserialize_(ReadBufferInterface& buffer) { return WireFormatter::DeserializeDouble(buffer, get()); }

  };


  using int32 = FieldTemplate<Field::FieldTypes::int32, int32_t, WireFormatter::WireType::VARINT>;
  using int64 = FieldTemplate<Field::FieldTypes::int64, int64_t, WireFormatter::WireType::VARINT>;
  using uint32 = FieldTemplate<Field::FieldTypes::uint32, uint32_t, WireFormatter::WireType::VARINT>; 
  using uint64 = FieldTemplate<Field::FieldTypes::uint64, uint64_t, WireFormatter::WireType::VARINT>; 
  using sint32 = FieldTemplate<Field::FieldTypes::sint32, int32_t, WireFormatter::WireType::VARINT>; 
  using sint64 = FieldTemplate<Field::FieldTypes::sint64, int64_t, WireFormatter::WireType::VARINT>; 
  using boolean = FieldTemplate<Field::FieldTypes::boolean, bool, WireFormatter::WireType::VARINT>; 
  using fixed32 = FieldTemplate<Field::FieldTypes::fixed32, uint32_t, WireFormatter::WireType::FIXED32>; 
  using fixed64 = FieldTemplate<Field::FieldTypes::fixed64, uint64_t, WireFormatter::WireType::FIXED64>; 
  using sfixed32 = FieldTemplate<Field::FieldTypes::sfixed32, int32_t, WireFormatter::WireType::FIXED32>; 
  using sfixed64 = FieldTemplate<Field::FieldTypes::sfixed64, int64_t, WireFormatter::WireType::FIXED64>; 
  using floatfixed = FieldTemplate<Field::FieldTypes::floatfixed, float, WireFormatter::WireType::FIXED32>; 
  using doublefixed = FieldTemplate<Field::FieldTypes::doublefixed, double, WireFormatter::WireType::FIXED64>;

  template<class ENUM_TYPE>
  using enumeration = FieldTemplate<Field::FieldTypes::enumeration, ENUM_TYPE, WireFormatter::WireType::VARINT>;

} // End of namespace EmbeddedProto.
#endif
