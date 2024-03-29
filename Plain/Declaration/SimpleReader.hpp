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

#ifndef _SIMPLEREADER_HPP_
#define _SIMPLEREADER_HPP_

#include <ReferenceCounting/DefaultImpl.hpp>
#include "BaseReader.hpp"

namespace Declaration
{
  class SimpleReader: public virtual BaseReader
  {
  public:
    struct CppReadTraits
    {
      CppReadTraits(
        const char* read_type_name_val,
        const char* read_type_cast_val,
        bool read_type_cast_with_size_val,
        const char* read_type_val,
        const char* field_type_suffix_val = "")
        : read_type_name(read_type_name_val),
          read_type_cast(read_type_cast_val),
          read_type_cast_with_size(read_type_cast_with_size_val),
          read_type(read_type_val),
          field_type_suffix(field_type_suffix_val)
      {}

      // read accessor return type
      std::string read_type_name;
      // (const void*, unsigned long buf_size) -> read_type_name
      std::string read_type_cast;
      bool read_type_cast_with_size;
      std::string read_type;
      std::string field_type_suffix;
    };

    SimpleReader(
      const char* name_val,
      const CppReadTraits& cpp_read_traits)
      throw();

    /* BaseReader */
    virtual SimpleReader_var as_simple_reader() throw();

    const CppReadTraits& cpp_read_traits() const throw();

  protected:
    virtual ~SimpleReader() throw() {}

  private:
    CppReadTraits cpp_read_traits_;
  };

  typedef ReferenceCounting::SmartPtr<SimpleReader>
    SimpleReader_var;
}

namespace Declaration
{
  inline
  SimpleReader::SimpleReader(
    const char* name_val,
    const CppReadTraits& cpp_read_traits)
    throw()
    : BaseType(name_val),
      BaseReader(name_val),
      cpp_read_traits_(cpp_read_traits)
  {}

  inline
  SimpleReader_var
  SimpleReader::as_simple_reader() throw()
  {
    return ReferenceCounting::add_ref(this);
  }

  inline
  const SimpleReader::CppReadTraits&
  SimpleReader::cpp_read_traits() const throw()
  {
    return cpp_read_traits_;
  }
}

#endif /*_SIMPLEREADER_HPP_*/
