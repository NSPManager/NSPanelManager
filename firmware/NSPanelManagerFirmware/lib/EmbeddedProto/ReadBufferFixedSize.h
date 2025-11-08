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

#ifndef _READ_BUFFER_FIXED_H_
#define _READ_BUFFER_FIXED_H_

#include "ReadBufferInterface.h"
#include <array>

namespace EmbeddedProto 
{

  //! This template class implements the ReadBufferInterface.
  /*!
      The template sets the number of bytes which fit in the buffer.
  */
  template<uint32_t BUFFER_SIZE>
  class ReadBufferFixedSize : public ::EmbeddedProto::ReadBufferInterface
  {
    public:
      //! The default constructor which initializes everything at zero.
      ReadBufferFixedSize() = default;

      //! The default destructor.
      ~ReadBufferFixedSize() override = default;

      //! \see ::EmbeddedProto::ReadBufferInterface::get_size()
      uint32_t get_size() const override
      {
        return write_index_;
      }

      //! \see ::EmbeddedProto::ReadBufferInterface::get_max_size()
      uint32_t get_max_size() const override
      {
        return BUFFER_SIZE;
      }

      //! \see ::EmbeddedProto::ReadBufferInterface::peak()
      bool peek(uint8_t& byte) const override
      {
        const bool return_value = write_index_ > read_index_;
        if(return_value)
        {
          byte = data_[read_index_];
        }
        return return_value;
      }

      //! \see ::EmbeddedProto::ReadBufferInterface::advance()
      bool advance() override
      {
        const bool return_value = write_index_ > read_index_;
        if(return_value)
        {
          ++read_index_;
        }
        return return_value;
      }

      //! \see ::EmbeddedProto::ReadBufferInterface::advance(const uint32_t N)
      bool advance(const uint32_t N) override
      {
        const uint32_t new_read_index = read_index_ + N;
        const bool return_value = write_index_ >= new_read_index;
        if(return_value)
        {
          read_index_ = new_read_index;
        }
        return return_value;
      }

      //! \see ::EmbeddedProto::ReadBufferInterface::pop()
      bool pop(uint8_t& byte) override
      {
        const bool return_value = write_index_ > read_index_;
        if(return_value)
        {
          byte = data_[read_index_];
          ++read_index_;
        }
        return return_value;
      }

      //! Return a pointer to the data array. Use set_bytes_written() when adding data to the array.
      uint8_t* get_data()
      {
        return data_.data();
      }

      //! Set the amount of bytes copied to the buffer when using get_data_array().
      void set_bytes_written(const uint32_t n_bytes)
      {
        write_index_ = std::min(n_bytes, BUFFER_SIZE);
      }

      //! Clear all indices, in effect allowing the data to be overwritten.
      void clear()
      {
        read_index_ = 0;
        write_index_ = 0;
      }

      //! Push a signle byte of data into the buffer.
      /*!
        \tip If you receive a whole message at once consider using get_data_array() in combination 
             with set_bytes_written().
      */
      bool push(const uint8_t& byte)
      {
        const bool return_value = BUFFER_SIZE > write_index_;
        if(return_value)
        {
          data_[write_index_] = byte;
          ++write_index_;
        }
        return return_value;
      }

    private:

      //! The array in which the data received over uart is stored.
      std::array<uint8_t, BUFFER_SIZE> data_ = {0};

      //! The number of bytes currently received and stored in the data array.
      uint32_t write_index_ = 0;

      //! The number of bytes read from the data array.
      uint32_t read_index_ = 0;
  };

} // namespace EmbeddedProto

#endif // End of _READ_BUFFER_FIXED_H_