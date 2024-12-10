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

#ifndef _FIELD_STRING_BYTES_H_
#define _FIELD_STRING_BYTES_H_

#include "Defines.h"
#include "Fields.h"
#include "Errors.h"

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <array>


namespace EmbeddedProto
{

  namespace internal
  {

    class BaseStringBytes : public Field {};

    template<uint32_t MAX_LENGTH, class DATA_TYPE>
    class FieldStringBytes : public BaseStringBytes
    {
      static_assert(std::is_same<uint8_t, DATA_TYPE>::value || std::is_same<char, DATA_TYPE>::value, 
                    "This class only supports unit8_t or chars.");

      public:

        FieldStringBytes() = default;
        
        ~FieldStringBytes() override = default;
        
        //! Obtain the number of characters in the string right now.
        uint32_t get_length() const { return current_length_; }

        //! Obtain the maximum number characters in the string.
        uint32_t get_max_length() const { return MAX_LENGTH; }

        //! Get a constant pointer to the first element in the array.
        const DATA_TYPE* get_const() const { return data_.data(); }

        //! Get a reference to the value at the given index. 
        /*!
          This function will update the number of elements used in the array/string.

          \param[in] index The desired index to return.
          \return The reference to the value at the given index. Will return the last element if the 
                  index is out of bounds
        */
        DATA_TYPE& get(uint32_t index) 
        { 
          uint32_t limited_index = std::min(index, MAX_LENGTH-1);
          // Check if we need to update the number of elements in the array.
          if(limited_index >= current_length_) {
            current_length_ = limited_index + 1;
          }
          return data_[limited_index]; 
        }

        //! Get a constant reference to the value at the given index. 
        /*!
          \param[in] index The desired index to return.
          \return The reference to the value at the given index. Will return the last element if the 
                  index is out of bounds
        */
        const DATA_TYPE& get_const(uint32_t index) const 
        { 
          uint32_t limited_index = std::min(index, MAX_LENGTH-1);
          return data_[limited_index]; 
        }

        //! Get a constant reference to the value at the given index.
        /*!
          \param[in] index The desired index to return.
          \param[out] value The value of the desired index is set in this reference.
          \return An error incase of an index out of bound situation.
        */
        Error get_const(const uint32_t index, DATA_TYPE& value) const
        {
          Error result = Error::NO_ERRORS;
          if(index < current_length_)
          {
            value = data_[index];
          }
          else
          {
            result = Error::INDEX_OUT_OF_BOUND;
          }
          return result;
        }

        //! Get a reference to the value at the given index. 
        /*!
          This function will update the number of elements used in the array/string.

          \param[in] index The desired index to return.
          \return The reference to the value at the given index. Will return the last element if the 
                  index is out of bounds
        */
        DATA_TYPE& operator[](uint32_t index) { return this->get(index); }

        //! Get a constant reference to the value at the given index. 
        /*!
          \param[in] index The desired index to return.
          \return The reference to the value at the given index. Will return the last element if the 
                  index is out of bounds
        */
        const DATA_TYPE& operator[](uint32_t index) const { return this->get_const(index); }

        //! Assign the values in the right hand side FieldStringBytes object to this object.
        /*!
            This is only compatible with the same data type and length.
            \param[in] rhs The object from which to copy the data.
            \return Always return NO_ERRORS, this was added to be compadible with the other set function.
        */
        template<uint32_t RHS_LENGTH> 
        Error set(const FieldStringBytes<RHS_LENGTH, DATA_TYPE>& rhs)
        {
          return this->set(rhs.get_const(), rhs.get_length());
        }

        //! Assign data in the given array to this object.
        /*!
            \param[in] data A pointer to an array with data.
            \param[in] length The number of bytes/chars in the data array.
            \return Will return ARRAY_FULL when length exceeds the number of bytes/chars in this object.
        */        
        Error set(const DATA_TYPE* data, const uint32_t length)
        {
          Error return_value = Error::NO_ERRORS;
          if(MAX_LENGTH >= length)
          {
            current_length_ = length;
            memcpy(data_.data(), data, length);
          }
          else
          {
            return_value = Error::ARRAY_FULL;
          }
          return return_value;
        }


        Error serialize_with_id(uint32_t field_number, WriteBufferInterface& buffer, const bool optional) const override 
        {
          Error return_value = Error::NO_ERRORS;

          if((0 < current_length_) || optional) 
          {
            const auto n_bytes_available = buffer.get_available_size();
            if(current_length_ <= n_bytes_available)
            {
              uint32_t tag = WireFormatter::MakeTag(field_number, 
                                                    WireFormatter::WireType::LENGTH_DELIMITED);
              return_value = WireFormatter::SerializeVarint(tag, buffer);
              if(Error::NO_ERRORS == return_value) 
              {
                return_value = WireFormatter::SerializeVarint(current_length_, buffer);
              }
              // Check check the number of elements again for optional fields.
              if((Error::NO_ERRORS == return_value) && (0 < current_length_)) 
              {
                return_value = serialize(buffer);
              }
            }
            else 
            {
              return_value = Error::BUFFER_FULL;
            }
          }

          return return_value;
        }

        Error serialize(WriteBufferInterface& buffer) const override 
        { 
          Error return_value = Error::NO_ERRORS;
          const auto* void_pointer = static_cast<const void*>(&(data_[0]));
          const auto* byte_pointer = static_cast<const uint8_t*>(void_pointer);
          if(!buffer.push(byte_pointer, current_length_))
          {
            return_value = Error::BUFFER_FULL;
          }
          return return_value;
        }

        Error deserialize(ReadBufferInterface& buffer) override 
        {
          uint32_t availiable = 0;
          Error return_value = WireFormatter::DeserializeVarint(buffer, availiable);
          if(Error::NO_ERRORS == return_value)
          {
            if(MAX_LENGTH >= availiable) 
            {
              clear();

              uint8_t byte = 0;
              while((current_length_ < availiable) && buffer.pop(byte)) 
              {
                (data_[current_length_]) = static_cast<DATA_TYPE>(byte);
                ++current_length_;
              }

              if(current_length_ != availiable)
              {
                // If at the end we did not read the same number of characters something went wrong.
                return_value = Error::END_OF_BUFFER;
              }
            }
            else 
            {
              return_value = Error::ARRAY_FULL;
            }
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

        //! Reset the field to it's initial value.
        void clear() override 
        { 
          data_.fill(0);
          current_length_ = 0; 
        }
       
      protected:

        //! Set the current number of items in the array. Only for internal usage.
        /*!
            The value is limited to the maximum lenght of the array.
        */
        void set_length(uint32_t length) { current_length_ = std::min(length, MAX_LENGTH); }

        //! Get a non constant pointer to the first element in the array. Only for internal usage.
        DATA_TYPE* get() { return data_.data(); }

      private:

        //! Number of item in the data array.
        uint32_t current_length_ = 0;

        //! The text.
        std::array<DATA_TYPE, MAX_LENGTH> data_ = {{0}};

    }; // End of class FieldStringBytes

  } // End of namespace internal

  //! The class definition used in messages for String fields.
  template<uint32_t MAX_LENGTH>
  class FieldString : public internal::FieldStringBytes<MAX_LENGTH, char>
  {
    public:

      using internal::FieldStringBytes<MAX_LENGTH, char>::set;

      FieldString() = default;
      ~FieldString() override = default;

      //! Assign the values in the right hand side FieldStringBytes object to this object.
      /*!
          This is only compatible with the same data type and length.
          \param[in] rhs The object from which to copy the data.
          \return A reference to this object.
      */
      template<uint32_t RHS_LENGTH> 
      FieldString<MAX_LENGTH>& operator=(const FieldString<RHS_LENGTH>& rhs)
      {
        this->set(rhs.get_const(), rhs.get_length());
        return *this;
      }

      //! Assign a c style string to this object.
      /*!
          A short example:
            char text[] = "Foo bar";
            msg.mutable_txt() = text;
          
          \param[in] rhs The c style string from which to take the characters and copy it to this object.
          \return A reference to this object used for function chaining.
      */
      FieldString<MAX_LENGTH>& operator=(const char* const rhs)
      {
        this->set(rhs);
        return *this;
      }

      //! Assign the data from the given c style string to this object.
      /*!
          \param[in] str The c style string from which to take the characters and copy it to this object.
      */
      void set(const char* const str)
      {
        if(nullptr != str) {
          const uint32_t str_MAX_LENGTH = strnlen(str, MAX_LENGTH + 1);
          this->set_length(str_MAX_LENGTH);
          uint32_t this_length = this->get_length();
          // If it fits in this object copy the null terminator.
          if(MAX_LENGTH > this_length) {
            ++this_length;
          }
          strncpy(this->get(), str, this_length);
        }
        else {
          this->clear();
        }      
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

        if(nullptr != name)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": \"%s\"", indent_level, " ", name, this->get_const());
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\"", indent_level, " ", this->get_const());
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
      //! Use our own implementation of limited string length function.
      /*!
          \param s The character array.
          \param len The maximum length to search for a null terminator.
          
          \return The length of this character array will be returned or the value of len.
      */
      uint32_t strnlen(const char* s, uint32_t len) 
      {
        uint32_t i = 0;
        for(; (i < len) && (s[i] != '\0'); ++i)
        {
          // Do nothing but the loop checks.
        }
        return i;
      }

  };

  //! The class definition used in messages for Bytes fields.
  template<uint32_t MAX_LENGTH>
  class FieldBytes : public internal::FieldStringBytes<MAX_LENGTH, uint8_t>
  {
    public:
      FieldBytes() = default;
      ~FieldBytes() override = default;

      //! Assign the values in the right hand side FieldStringBytes object to this object.
      /*!
          This is only compatible with the same data type and length.
          \param[in] rhs The object from which to copy the data.
          \return A reference to this object.
      */
      template<uint32_t RHS_LENGTH> 
      FieldBytes<MAX_LENGTH>& operator=(const FieldBytes<RHS_LENGTH>& rhs)
      {
        this->set(rhs.get_const(), rhs.get_length());
        return *this;
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

        if(nullptr != name)
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s\"%s\": [\n", indent_level, " ", name );
        }
        else
        {
          n_chars_used = snprintf(left_chars.data, left_chars.size, "%*s[\n", indent_level, " ");
        }
        
        if(0 < n_chars_used) 
        {
          left_chars.data += n_chars_used;
          left_chars.size -= n_chars_used;
        }

        uint32 field;
        for(uint32_t i = 0; i < this->get_length(); ++i)
        {
          field = this->get_const(i);
          left_chars = field.to_string(left_chars, n_chars_used, nullptr, (0 == i));
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
  };


} // End of namespace EmbeddedProto

#endif // End of _FIELD_STRING_BYTES_H_
