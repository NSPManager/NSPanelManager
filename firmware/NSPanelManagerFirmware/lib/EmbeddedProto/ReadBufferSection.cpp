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

#include "ReadBufferSection.h"

#include <algorithm>


namespace EmbeddedProto 
{

  ReadBufferSection::ReadBufferSection(ReadBufferInterface& buffer, const uint32_t size)
    : buffer_(buffer),
      size_(std::min(size, buffer.get_size())),
      max_size_(std::min(size, buffer.get_size()))
  {
    
  }

  uint32_t ReadBufferSection::get_size() const
  {
    return size_;
  }

  uint32_t ReadBufferSection::get_max_size() const
  {
    return max_size_;
  }

  bool ReadBufferSection::peek(uint8_t& byte) const
  {
    bool result = 0 < size_;
    if(result)
    {
      result = buffer_.peek(byte);
    }
    return result;
  }

  bool ReadBufferSection::advance()
  {
    bool result = 0 < size_;
    if(result) 
    {
      result = buffer_.advance();
      --size_;
    }
    return result;
  }

  bool ReadBufferSection::advance(const uint32_t n_bytes)
  {
    bool result = 0 < size_;
    if(result) 
    {
      uint32_t n = (n_bytes <= size_) ? n_bytes : size_;
      result = buffer_.advance(n);
      size_ -= n;
    }
    return result;
  }

  bool ReadBufferSection::pop(uint8_t& byte)
  {
    bool result = 0 < size_;
    if(result)
    {
      result = buffer_.pop(byte);
      --size_;
    }
    return result;
  }

} // End of namespace EmbeddedProto
