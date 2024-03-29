/* 
 * This file is part of the RTBServer distribution (https://github.com/yoori/rtbserver).
 * RTBServer is DSP server that allow to bid (see RTB auction) targeted ad
 * via RTB protocols (OpenRTB, Google AdExchange, Yandex RTB)
 *
 * Copyright (c) 2014 Yuri Kuznecov <yuri.kuznecov@gmail.com>.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU General Public License as published by  
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PLAIN_BUFFER_HPP
#define PLAIN_BUFFER_HPP

#include "Base.hpp"
#include <Generics/MemBuf.hpp>

namespace PlainTypes
{
  struct Buffer: public Generics::MemBuf
  {
  public:
    static const unsigned long FIXED_SIZE = 8;

    void init_default() throw();

    void init(const void* buf, unsigned long size)
      throw(CorruptedStruct);

    unsigned long dyn_size_() const throw();

    void save_(void* fixed_buf, void* dyn_buf) const
      throw();

    static ConstBuf read_cast(const void* fixed_buf);

    Buffer&
    operator=(const Buffer& right) throw();
  };
}

namespace PlainTypes
{
  // Buffer
  inline
  void
  Buffer::init_default() throw()
  {}

  inline
  void
  Buffer::init(const void* buf, unsigned long size)
    throw(CorruptedStruct)
  {
    static const char* FUN = "Buffer::init()";

    uint32_t buf_offset = *static_cast<const uint32_t*>(buf);
    uint32_t buf_size = *(static_cast<const uint32_t*>(buf) + 1);

    if(buf_offset + buf_size > size)
    {
      Stream::Error ostr;
      ostr << FUN << ": buffer end position great than size: " <<
        (buf_offset + buf_size) << " > " << size;
      throw CorruptedStruct(ostr);
    }

    assign(static_cast<const char*>(buf) + buf_offset, buf_size);
  }

  inline
  unsigned long
  Buffer::dyn_size_() const throw()
  {
    return size();
  }

  inline
  void
  Buffer::save_(void* fixed_buf, void* dyn_buf) const
    throw()
  {
    *static_cast<uint32_t*>(fixed_buf) =
      static_cast<unsigned char*>(dyn_buf) -
      static_cast<unsigned char*>(fixed_buf);
    *(static_cast<uint32_t*>(fixed_buf) + 1) = size();
    ::memcpy(dyn_buf, data(), size());
  }

  inline
  ConstBuf
  Buffer::read_cast(const void* fixed_buf)
  {
    return ConstBuf(
      static_cast<const char*>(fixed_buf) +
        *static_cast<const uint32_t*>(fixed_buf),
      *(static_cast<const uint32_t*>(fixed_buf) + 1));
  }

  inline
  Buffer&
  Buffer::operator=(const Buffer& right) throw()
  {
    assign(right.data(), right.size());
    return *this;
  }
}

#endif /*PLAIN_BUFFER_HPP*/
