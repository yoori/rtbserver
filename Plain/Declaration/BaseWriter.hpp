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

#ifndef _BASEWRITER_HPP_
#define _BASEWRITER_HPP_

#include <list>
#include <set>
#include <eh/Exception.hpp>
#include "BaseType.hpp"

namespace Declaration
{
  typedef std::set<std::string> MappingSpecifierSet;

  class SimpleWriter;
  typedef ReferenceCounting::SmartPtr<SimpleWriter>
    SimpleWriter_var;

  class StructWriter;
  typedef ReferenceCounting::SmartPtr<StructWriter>
    StructWriter_var;

  class BaseWriter: public virtual BaseType
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(InvalidMappingSpecifier, Exception);

    BaseWriter(const char* name_val) throw();

    virtual BaseDescriptor_var descriptor() throw() = 0;

    virtual void
    check_mapping_specifiers(
      const MappingSpecifierSet& mapping_specifiers)
      throw(InvalidMappingSpecifier) = 0;

    /* non fixed field */
    virtual SimpleWriter_var as_simple_writer() throw();

    virtual StructWriter_var as_struct_writer() throw();

    virtual BaseWriter_var as_writer() throw();

  protected:
    virtual ~BaseWriter() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<BaseWriter>
    BaseWriter_var;

  typedef std::list<BaseWriter_var> BaseWriterList;
}

#endif /*_BASEWRITER_HPP_*/
