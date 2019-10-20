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

#ifndef PLAIN_STRING_HPP
#define PLAIN_STRING_HPP

#include <string>
#include <cstring>
#include <Stream/MemoryStream.hpp>

namespace PlainTypes
{
  struct String: public std::string
  {
    static const unsigned long FIXED_SIZE = 4;

    String();

    String(const std::string& init);

    String(const char* init);

    void init_default() throw();

    void unsafe_init(const void* buf, unsigned long size)
      throw(CorruptedStruct);

    void init(const void* buf, unsigned long size)
      throw(CorruptedStruct);

    unsigned long dyn_size_() const throw();

    void save_(void* fixed_buf, void* dyn_buf) const
      throw();

    static const char* read_cast(
      const void* fixed_buf,
      const void* /*end_buf*/ = 0);
  };
}

namespace PlainTypes
{
  inline
  String::String()
  {}

  inline
  String::String(const std::string& init)
    : std::string(init)
  {}

  inline
  String::String(const char* init)
    : std::string(init)
  {}

  inline
  void
  String::init_default() throw()
  {}

  inline
  void
  String::unsafe_init(const void* buf, unsigned long size)
    throw(CorruptedStruct)
  {
    static const char* FUN = "String::unsafe_init()";

    uint32_t offset = *static_cast<const uint32_t*>(buf);

    if(offset > size)
    {
      Stream::Error ostr;
      ostr << FUN << ": string position great than size: " <<
        offset << " > " << size;
      throw CorruptedStruct(ostr);
    }

    std::string::operator=(static_cast<const char*>(buf) + offset);
  }

  inline
  void
  String::init(const void* buf, unsigned long size)
    throw(CorruptedStruct)
  {
    static const char* FUN = "String::init()";

    if(size < 4)
    {
      Stream::Error ostr;
      ostr << FUN << ": buffer size = " << size << " is less then string header size";
      throw CorruptedStruct(ostr);
    }

    unsafe_init(buf, size);
  }

  inline
  unsigned long
  String::dyn_size_() const throw()
  {
    return size() + 1;
  }

  inline
  void
  String::save_(void* fixed_buf, void* dyn_buf) const
    throw()
  {
    *static_cast<uint32_t*>(fixed_buf) =
      static_cast<unsigned char*>(dyn_buf) -
      static_cast<unsigned char*>(fixed_buf);

    ::memcpy(dyn_buf, c_str(), size() + 1);
  }

  inline
  const char*
  String::read_cast(
    const void* fixed_buf,
    const void* /*end_buf*/)
  {
    return static_cast<const char*>(fixed_buf) +
      *static_cast<const uint32_t*>(fixed_buf);
  }
}

#endif /*PLAIN_STRING_HPP*/
