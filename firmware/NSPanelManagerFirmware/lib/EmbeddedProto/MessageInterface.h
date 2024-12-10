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

#ifndef _MESSAGE_INTERFACE_H_
#define _MESSAGE_INTERFACE_H_

#include "WireFormatter.h"
#include "Fields.h"
#include "Errors.h"

#include <cstdint>


namespace EmbeddedProto 
{

class MessageInterface : public ::EmbeddedProto::Field
{
  public:

    MessageInterface() = default;

    ~MessageInterface() override = default;

    //! \see Field::serialize_with_id()
    Error serialize_with_id(uint32_t field_number, 
                            ::EmbeddedProto::WriteBufferInterface& buffer,
                            const bool optional) const final;

    //! \see Field::deserialize()
    Error deserialize(::EmbeddedProto::ReadBufferInterface& buffer) override = 0;

    //! \see Field::deserialize()
    Error deserialize_check_type(::EmbeddedProto::ReadBufferInterface& buffer, 
                                 const ::EmbeddedProto::WireFormatter::WireType& wire_type) final;

    //! Clear the content of this message and set it to it's default state.
    /*!
        The defaults are to be set according to the Protobuf standard.
    */
    void clear() override = 0;
    

  protected:
    //! When deserializing skip the bytes in the buffer of an unknown field.
    /*! 
        This function is used when a field with an unknown id is encountered to move through the 
        buffer to the next tag.
    */
    Error skip_unknown_field(::EmbeddedProto::ReadBufferInterface& buffer, 
                             const ::EmbeddedProto::WireFormatter::WireType& wire_type) const;

    Error skip_varint(::EmbeddedProto::ReadBufferInterface& buffer) const;
    Error skip_fixed32(::EmbeddedProto::ReadBufferInterface& buffer) const;
    Error skip_fixed64(::EmbeddedProto::ReadBufferInterface& buffer) const;
    Error skip_length_delimited(::EmbeddedProto::ReadBufferInterface& buffer) const;

};

} // End of namespace EmbeddedProto

#endif // _MESSAGE_INTERFACE_H_
