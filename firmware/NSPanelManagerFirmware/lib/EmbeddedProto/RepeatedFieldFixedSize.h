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

#ifndef _REPEATED_FIELD_SIZE_H_
#define _REPEATED_FIELD_SIZE_H_

#include "RepeatedField.h"
#include "Errors.h"

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <array>


namespace EmbeddedProto
{

  //! A template class that actually holds some data.
  /*!
    This is a separate class to make it possible to not have the size defined in every function or 
    class using this type of object.
  */
  template<class DATA_TYPE, uint32_t MAX_LENGTH>
  class RepeatedFieldFixedSize : public RepeatedField<DATA_TYPE>
  { 
      static constexpr uint32_t BYTES_PER_ELEMENT = sizeof(DATA_TYPE);

    public:

      RepeatedFieldFixedSize() = default;
      ~RepeatedFieldFixedSize() override = default;

      RepeatedFieldFixedSize(const RepeatedFieldFixedSize<DATA_TYPE, MAX_LENGTH>& rhs) :
        current_length_(rhs.get_length()),
        data_(rhs.get_data_const())
      {
        // Use the initializer list.
      }

      template<uint32_t MAX_LENGTH_RHS, typename std::enable_if<(MAX_LENGTH_RHS < MAX_LENGTH), int>::type = 0>
      explicit RepeatedFieldFixedSize(const RepeatedFieldFixedSize<DATA_TYPE, MAX_LENGTH_RHS>& rhs) :
        current_length_(rhs.get_length())
      {
        const auto& rhs_data = rhs.get_data_const();
        std::copy(rhs_data.begin(), rhs_data.end(), data_.begin());
      }

      template<uint32_t MAX_LENGTH_RHS, typename std::enable_if<(MAX_LENGTH_RHS < MAX_LENGTH), int>::type = 0>
      explicit RepeatedFieldFixedSize(const RepeatedFieldFixedSize<DATA_TYPE, MAX_LENGTH_RHS>&& rhs) :
        current_length_(rhs.get_length())
      {
        const auto& rhs_data = rhs.get_data_const();
        std::copy(rhs_data.begin(), rhs_data.end(), data_.begin());
      }

      //! Assign one repieted field to the other, but only when the length and type matches.
      RepeatedFieldFixedSize<DATA_TYPE, MAX_LENGTH>& operator=(const 
                                                RepeatedFieldFixedSize<DATA_TYPE, MAX_LENGTH>& rhs)
      {
        for(uint32_t i = 0; i < rhs.get_length(); ++i) 
        {
          data_[i] = rhs.get_const(i);
        }
        current_length_ = rhs.get_length();
        
        return *this;
      }

      //! Obtain the total number of DATA_TYPE items in the array.
      uint32_t get_length() const override { return current_length_; }

      //! Obtain the maximum number of DATA_TYPE items which can at most be stored in the array.
      uint32_t get_max_length() const override { return MAX_LENGTH; }

      //! Obtain the total number of bytes currently stored in the array.
      uint32_t get_size() const override { return BYTES_PER_ELEMENT * current_length_; }

      //! Obtain the maximum number of bytes which can at most be stored in the array.
      uint32_t get_max_size() const override { return BYTES_PER_ELEMENT * MAX_LENGTH; }

      DATA_TYPE& get(uint32_t index) override 
      { 
        uint32_t limited_index = std::min(index, MAX_LENGTH-1);
        // Check if we need to update the number of elements in the array.
        if(limited_index >= current_length_) {
          current_length_ = limited_index + 1;
        }
        return data_[limited_index]; 
      }

      const DATA_TYPE& get_const(uint32_t index) const override 
      { 
        uint32_t limited_index = std::min(index, MAX_LENGTH-1);
        return data_[limited_index]; 
      }

      Error get_const(const uint32_t index, DATA_TYPE& value) const override
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

      void set(uint32_t index, const DATA_TYPE& value) override 
      { 
        uint32_t limited_index = std::min(index, MAX_LENGTH-1);
        // Check if we need to update the number of elements in the array.
        if(limited_index >= current_length_) {
          current_length_ = limited_index + 1;
        }
        data_[limited_index] = value;  
      }

      Error set_data(const DATA_TYPE* data, const uint32_t length) override 
      {
        Error return_value = Error::NO_ERRORS;
        if(MAX_LENGTH >= length) 
        {
          const DATA_TYPE* d = data;
          for(uint32_t i = 0; i < length; ++i) 
          {
            (data_[i]) = (*d);
            ++d;
          }
          current_length_ = length;        
        }
        else 
        {
          return_value = Error::ARRAY_FULL;
        }
        return return_value;
      }

      Error add(const DATA_TYPE& value) override 
      {
        Error return_value = Error::NO_ERRORS;
        if(MAX_LENGTH > current_length_) 
        {
          data_[current_length_] = value;
          ++current_length_;
        }
        else 
        {
          return_value = Error::ARRAY_FULL;
        }
        return return_value;
      }

      void clear() override 
      {
        for(auto& d : data_)
        {
          d.clear();
        }
        current_length_ = 0;
      }

      //! Return a reference to the internal data storage array.
      const std::array<DATA_TYPE, MAX_LENGTH>& get_data_const() const { return data_; }

    private:

      //! Number of item in the data array.
      uint32_t current_length_ = 0;

      //! The actual data 
      std::array<DATA_TYPE, MAX_LENGTH> data_ = {};
  };

} // End of namespace EmbeddedProto

#endif // End of _REPEATED_FIELD_SIZE_H_