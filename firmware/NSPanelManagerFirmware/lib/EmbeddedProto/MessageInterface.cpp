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

#include "MessageInterface.h"
#include "WireFormatter.h"
#include "ReadBufferSection.h"

namespace EmbeddedProto
{

  Error MessageInterface::MessageInterface::serialize_with_id(uint32_t field_number, 
                                                              ::EmbeddedProto::WriteBufferInterface& buffer,
                                                              const bool optional) const
  {
    Error return_value = Error::NO_ERRORS;

    // See if we have data which should be serialized.
    const uint32_t size_x = this->serialized_size();
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
          // See if there is enough space left in the buffer for the data.
          if(size_x <= buffer.get_available_size()) 
          {
            const auto* base = static_cast<const ::EmbeddedProto::Field*>(this);  
            return_value = base->serialize(buffer);
          }
          else
          {
            return_value = Error::BUFFER_FULL;
          }
        }
      }
    }
    return return_value;
  }


  Error MessageInterface::deserialize_check_type(::EmbeddedProto::ReadBufferInterface& buffer,
                                                 const ::EmbeddedProto::WireFormatter::WireType& wire_type)
  {
    Error return_value = ::EmbeddedProto::WireFormatter::WireType::LENGTH_DELIMITED == wire_type 
                         ? Error::NO_ERRORS : Error::INVALID_WIRETYPE;
    if(Error::NO_ERRORS == return_value)  
    {
      uint32_t size = 0;
      return_value = ::EmbeddedProto::WireFormatter::DeserializeVarint(buffer, size);
      ::EmbeddedProto::ReadBufferSection bufferSection(buffer, size);
      if(::EmbeddedProto::Error::NO_ERRORS == return_value)
      {
        return_value = deserialize(bufferSection);
      }
    }
    return return_value;
  }


  Error MessageInterface::skip_unknown_field(::EmbeddedProto::ReadBufferInterface& buffer,
                                             const ::EmbeddedProto::WireFormatter::WireType& wire_type) const
  {
    Error return_value = Error::NO_ERRORS;

    // Depending on the wire type select one of its valid variable types and deserialize the value.
    switch(wire_type) {
      case ::EmbeddedProto::WireFormatter::WireType::VARINT:
        return_value = skip_varint(buffer);
        break;

      case ::EmbeddedProto::WireFormatter::WireType::FIXED64:
        return_value = skip_fixed64(buffer);
        break;

      case ::EmbeddedProto::WireFormatter::WireType::LENGTH_DELIMITED:
        return_value = skip_length_delimited(buffer);
        break;

      case ::EmbeddedProto::WireFormatter::WireType::FIXED32:
        return_value = skip_fixed32(buffer);
        break;

      default:
        // We should never get here. DeserializeTag catches this case.
        break;
    }

    return return_value;
  }


  Error MessageInterface::skip_varint(::EmbeddedProto::ReadBufferInterface& buffer) const
  {
    // Use a 64 bit variable to decode the maximum possible number of bytes. As we do not know
    // the actual type.
    uint64_t dummy;
    return ::EmbeddedProto::WireFormatter::DeserializeVarint(buffer, dummy);
  }

  Error MessageInterface::skip_fixed32(::EmbeddedProto::ReadBufferInterface& buffer) const
  {
    float dummy;
    return ::EmbeddedProto::WireFormatter::DeserializeFloat(buffer, dummy);
  }

  Error MessageInterface::skip_fixed64(::EmbeddedProto::ReadBufferInterface& buffer) const
  {
    double dummy;
    return ::EmbeddedProto::WireFormatter::DeserializeDouble(buffer, dummy);
  }

  Error MessageInterface::skip_length_delimited(::EmbeddedProto::ReadBufferInterface& buffer) const
  {
    // First read the number of bytes 
    uint32_t n_bytes = 0;
    const Error return_value = ::EmbeddedProto::WireFormatter::DeserializeVarint(buffer, n_bytes);
    if(Error::NO_ERRORS == return_value)
    {
      buffer.advance(n_bytes);
    }
    return return_value;
  }

} // End of namespace EmbeddedProto
