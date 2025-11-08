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
 *  Postal adress:
 *    Atoomweg 2
 *    1627 LE, Hoorn
 *    the Netherlands
 */

#ifndef _WRITE_BUFFER_FIXED_H_
#define _WRITE_BUFFER_FIXED_H_

#include "WriteBufferInterface.h"
#include <array>

namespace EmbeddedProto 
{

  template<uint32_t BUFFER_SIZE>
  class WriteBufferFixedSize : public ::EmbeddedProto::WriteBufferInterface
  {  
    public:
      WriteBufferFixedSize() = default;
      ~WriteBufferFixedSize() override = default;
  
      //! \see ::EmbeddedProto::WriteBufferInterface::clear()
      void clear() override
      {
        write_index_ = 0;
      }
  
  
      //! \see ::EmbeddedProto::WriteBufferInterface::get_size()
      uint32_t get_size() const override
      {
         return write_index_;
      }
  
  
      //! \see ::EmbeddedProto::WriteBufferInterface::get_max_size()
      uint32_t get_max_size() const override
      {
        return BUFFER_SIZE;
      }
  
      //! \see ::EmbeddedProto::WriteBufferInterface::get_available_size()
      uint32_t get_available_size() const override
      {
        return BUFFER_SIZE - write_index_;
      }
  
      //! \see ::EmbeddedProto::WriteBufferInterface::push()
      bool push(const uint8_t byte) override
      {
        bool return_value = BUFFER_SIZE > write_index_;
        if(return_value)
        {
          data_[write_index_] = byte;
          ++write_index_;
        }
        return return_value;
      }
  
      //! \see ::EmbeddedProto::WriteBufferInterface::push()
      bool push(const uint8_t* bytes, const uint32_t length) override
      {
        bool return_value = BUFFER_SIZE > (write_index_ + length);
        if(return_value)
        {
          memcpy(data_.data() + write_index_, bytes, length);
          write_index_ += length;
        }
        return return_value;
      }
  
      //! Return a pointer to the data array.
      uint8_t* get_data()
      {
        return data_.data();
      }
  
    private:
  
      //! The array in which the serialized data is stored.
      std::array<uint8_t, BUFFER_SIZE> data_ = {0};
  
      //! The number of bytes currently serialized in the array.
      uint32_t write_index_ = 0 ;
  
  };

} // End of namespace EmbeddedProto


#endif // _WRITE_BUFFER_FIXED_H_ 
