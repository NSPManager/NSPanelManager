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

#ifndef _EMBEDDED_PROTO_DEFINES_H_
#define _EMBEDDED_PROTO_DEFINES_H_

#include <type_traits>
#include <cstdint>

#if __cplusplus >= 201703L // C++17 and up
#include <memory>
#endif

namespace EmbeddedProto
{


#if __cplusplus >= 201703L // C++17 and up
  
  template<class T>
  inline constexpr void destroy_at(T* p) 
  {
    std::destroy_at(p);
  }

#elif __cplusplus >= 201402L // C++14

  template<class T>
  constexpr void destroy_at(T* p) 
  {
    p->~T(); 
  }

#elif __cplusplus >= 201103L // C++11

  template<class T>
  constexpr void destroy_at(T* p) 
  {
    p->~T(); 
  }

#else // Other

  #error "Unsupported version of C++. Embedded Proto supports C++11 and up."

#endif


  //! An simple struct holding both a pointer to an array and the size of that array.
  template<class T>
  struct array_view {
    T* data; //!< A pointer to the start of an array.
    uint32_t size; //!< The number of elements in the array.
  };

  using string_view = array_view<char>;
  using bytes_view = array_view<uint8_t>;

}

#endif //_EMBEDDED_PROTO_DEFINES_H_