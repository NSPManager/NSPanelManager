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

#ifndef _REPEATED_FIELD_H_
#define _REPEATED_FIELD_H_

#include "Defines.h"
#include "Fields.h"
#include "MessageInterface.h"
#include "MessageSizeCalculator.h"
#include "ReadBufferSection.h"
#include "FieldStringBytes.h"
#include "Errors.h"

#include <cstdint>


namespace EmbeddedProto
{

  //! Class template that specifies the interface of an arry with the data type.
  template<class DATA_TYPE>
  class RepeatedField : public Field
  {

    //! Definition of a trait to check if DATA_TYPE is NOT a specialization of the FieldTemplate.
    template<typename>
    struct is_specialization_of_FieldTemplate : std::false_type {};

    //! Definition of a trait to check if DATA_TYPE is a specialization of the FieldTemplate.
    template<Field::FieldTypes F, typename V, WireFormatter::WireType W>
    struct is_specialization_of_FieldTemplate<::EmbeddedProto::FieldTemplate<F,V,W>> : std::true_type {};

    //! This class only supports Field and FieldTemplate classes as template parameter.
    static_assert(std::is_base_of<::EmbeddedProto::Field, DATA_TYPE>::value || is_specialization_of_FieldTemplate<DATA_TYPE>::value, 
                  "A Field can only be used as template paramter.");

    //! Check how this field shoeld be serialized, packed or not.
    static constexpr bool REPEATED_FIELD_IS_PACKED = 
          !(std::is_base_of<MessageInterface, DATA_TYPE>::value
            || std::is_base_of<internal::BaseStringBytes, DATA_TYPE>::value);

    public:

      RepeatedField() = default;
      ~RepeatedField() override = default;

      //! Obtain the total number of DATA_TYPE items in the array.
      virtual uint32_t get_length() const = 0;

      //! Obtain the maximum number of DATA_TYPE items which can at most be stored in the array.
      virtual uint32_t get_max_length() const = 0;
      
      //! Obtain the total number of bytes currently stored in the array.
      virtual uint32_t get_size() const = 0;

      //! Obtain the maximum number of bytes which can at most be stored in the array.
      virtual uint32_t get_max_size() const = 0;

      //! Get a reference to the value at the given index. 
      /*!
        \param[in] index The desired index to return.
        \return The reference to the value at the given index.
      */
      virtual DATA_TYPE& get(uint32_t index) = 0;

      //! Get a constant reference to the value at the given index. 
      /*!
        \param[in] index The desired index to return.
        \return The constant reference to the value at the given index.
      */
      virtual const DATA_TYPE& get_const(uint32_t index) const = 0;

      //! Get a constant reference to the value at the given index.
      /*!
        \param[in] index The desired index to return.
        \param[out] value The value of the desired index is set in this reference.
        \return An error incase of an index out of bound situation.
      */
      virtual Error get_const(const uint32_t index, DATA_TYPE& value) const = 0;

      //! Get a reference to the value at the given index. 
      /*!
        \param[in] index The desired index to return.
        \return The reference to the value at the given index.
      */
      DATA_TYPE& operator[](uint32_t index) { return this->get(index); }

      //! Get a reference to the value at the given index. But constant. 
      /*!
        \param[in] index The desired index to return.
        \return The constant reference to the value at the given index.
      */
      const DATA_TYPE& operator[](uint32_t index) const { return this->get_const(index); }

      //! Set the value at the given index.
      /*!
        \param[in] index The desired index to change.
        \param[in] value The value we would like to set.
      */
      virtual void set(uint32_t index, const DATA_TYPE& value) = 0;

      //! Given a different array of known length copy that data into this object.
      /*!
        \param[in] data A pointer the array to copy from.
        \param[in] length The number of value of DATA_TYPE in the array.
        \return Error::NO_ERRORS when every was successful. Error::ARRAY_FULL when there is no space left.
      */
      virtual Error set_data(const DATA_TYPE* data, const uint32_t length) = 0;

      //! Append a value to the end of the array.
      /*!
        \param[in] value The data to add.
        \return Error::NO_ERRORS when every was successful. Error::ARRAY_FULL when there is no space left.
      */
      virtual Error add(const DATA_TYPE& value) = 0;

      //! Remove all data in the array and set it to the default value.
      virtual void clear() override = 0;

      Error serialize(WriteBufferInterface& buffer) const final
      {
        // This function should not be called on a repeated field.
        return Error::BUFFER_FULL;
      };

      //! \see Field::serialize_with_id()
      Error serialize_with_id(uint32_t field_number, WriteBufferInterface& buffer, const bool optional) const final
      {
        Error return_value = Error::NO_ERRORS;

        if(REPEATED_FIELD_IS_PACKED)
        {
          // Use the packed way of serialization for base fields.
          // See if there is data to serialize.
          const uint32_t size_x = this->serialized_size_packed();
          if((0 < size_x) || optional)
          {
            uint32_t tag = WireFormatter::MakeTag(field_number, 
                                          WireFormatter::WireType::LENGTH_DELIMITED);
            return_value = WireFormatter::SerializeVarint(tag, buffer);
            if(Error::NO_ERRORS == return_value) 
            {
              return_value = WireFormatter::SerializeVarint(size_x, buffer);

              if(Error::NO_ERRORS == return_value)
              {          
                if(size_x <= buffer.get_available_size()) 
                {
                  return_value = serialize_packed(buffer);
                }
                else
                {
                  return_value = Error::BUFFER_FULL;
                }
              }
            }
          }
        }
        else 
        {
          const uint32_t size_x = this->serialized_size_unpacked(field_number);
          if(size_x <= buffer.get_available_size()) 
          {
            return_value = serialize_unpacked(field_number, buffer);
          }
          else 
          {
            return_value = Error::BUFFER_FULL;
          }
        }

        return return_value;
      }

      //! Function to deserialize this array.
      /*!
          From a buffer of data fill this array with data.
          \param buffer [in]  The memory from which the message is obtained.
          \return Error::NO_ERRORS when every was successful. 
      */
      Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) final
      {
        Error return_value = Error::NO_ERRORS;
        if(REPEATED_FIELD_IS_PACKED)
        {              
          return_value = deserialize_packed(buffer);
        }
        else 
        {
          return_value = deserialize_unpacked(buffer);
        }
        return return_value;
      }

      Error deserialize_check_type(::EmbeddedProto::ReadBufferInterface& buffer, 
                                   const ::EmbeddedProto::WireFormatter::WireType& wire_type) final
      {
        Error return_value = ::EmbeddedProto::WireFormatter::WireType::LENGTH_DELIMITED == wire_type 
                             ? Error::NO_ERRORS : Error::INVALID_WIRETYPE;
        if(Error::NO_ERRORS == return_value)  
        {
          return_value = this->deserialize(buffer);
        }
        return return_value;
      }


      //! Calculate the size of this field when serialized.
      /*!
          The calculation only includes the data, not the size required by the tag and 
          \return The number of bytes this field will require once serialized.
      */
      uint32_t serialized_size_packed() const 
      {
        ::EmbeddedProto::MessageSizeCalculator calcBuffer;
        serialize_packed(calcBuffer);
        return calcBuffer.get_size();
      }

      //! Calculate the size of this field when serialized.
      /*!
          \return The number of bytes this field will require once serialized.
      */
      uint32_t serialized_size_unpacked(int32_t field_number) const 
      {
        ::EmbeddedProto::MessageSizeCalculator calcBuffer;
        serialize_unpacked(field_number, calcBuffer);
        return calcBuffer.get_size();
      }


#ifdef MSG_TO_STRING

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

        n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": [\n", indent_level, " ", name );
        
        if(0 < n_chars_used) 
        {
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }

        for(uint32_t i = 0; i < this->get_length(); ++i)
        {
          left_chars = this->get_const(i).to_string(left_chars, n_chars_used, nullptr, (0 == i));
        }

        n_chars_used = snprintf(left_chars.data, left_chars.size, "\n%*s]", n_chars_used - 2, " ");
        
        if(0 < n_chars_used)
        {
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }

        return left_chars;
      }

#endif // End of MSG_TO_STRING

    private:

      Error serialize_packed(WriteBufferInterface& buffer) const
      {
        Error return_value = Error::NO_ERRORS;
        for(uint32_t i = 0; (i < this->get_length()) && (Error::NO_ERRORS == return_value); ++i)
        {
          return_value = this->get_const(i).serialize(buffer);
        }
        return return_value;
      }

      Error serialize_unpacked(uint32_t field_number, WriteBufferInterface& buffer) const
      {
        Error return_value = Error::NO_ERRORS;
        for(uint32_t i = 0; (i < this->get_length()) && (Error::NO_ERRORS == return_value); ++i)
        {
          const uint32_t size_x = this->get_const(i).serialized_size();
          uint32_t tag = WireFormatter::MakeTag(field_number, 
                                    WireFormatter::WireType::LENGTH_DELIMITED);
          return_value = WireFormatter::SerializeVarint(tag, buffer);
          if(Error::NO_ERRORS == return_value)
          {
            return_value = WireFormatter::SerializeVarint(size_x, buffer);
            if((Error::NO_ERRORS == return_value) && (0 < size_x)) 
            {
              return_value = this->get_const(i).serialize(buffer);
            }
          }
        }
        return return_value;
      }

      Error deserialize_packed(ReadBufferInterface& buffer)
      {
        uint32_t size = 0;
        Error return_value = WireFormatter::DeserializeVarint(buffer, size);
        ReadBufferSection bufferSection(buffer, size);
        DATA_TYPE x;
        
        return_value = x.deserialize(bufferSection);
        while(Error::NO_ERRORS == return_value)
        {
          return_value = this->add(x);
          if(Error::NO_ERRORS == return_value)
          {
            return_value = x.deserialize(bufferSection);
          }
        }

        // We expect the buffersection to be empty, in that case everything is fine..
        if(Error::END_OF_BUFFER == return_value)
        {
          return_value = Error::NO_ERRORS;
        }

        return return_value;
      }

      Error deserialize_unpacked(ReadBufferInterface& buffer)
      {
        Error return_value = Error::NO_ERRORS;

        // For repeated messages, strings or bytes
        // First allocate an element in the array.
        const uint32_t index = this->get_length();
        if(this->get_max_length() > index)
        {
          // For messages read the size here, with strings and byte arrays this is include in 
          // deserialize.
          if(std::is_base_of<MessageInterface, DATA_TYPE>::value)
          {
            uint32_t size;
            return_value = WireFormatter::DeserializeVarint(buffer, size);
            if(Error::NO_ERRORS == return_value) 
            {
              ReadBufferSection bufferSection(buffer, size);
              return_value = this->get(index).deserialize(bufferSection);
            }
          }
          else 
          {
            return_value = this->get(index).deserialize(buffer);
          }
        }
        else 
        {
          return_value = Error::ARRAY_FULL;
        }

        return return_value;
      }

  };


} // End of namespace EmbeddedProto

#endif // End of _REPEATED_FIELD_H_
