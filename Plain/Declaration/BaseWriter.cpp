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

#include "BaseWriter.hpp"
#include "SimpleWriter.hpp"
#include "StructWriter.hpp"

namespace Declaration
{
  BaseWriter::BaseWriter(const char* name_val) throw()
    : BaseType(name_val)
  {}

  BaseWriter_var
  BaseWriter::as_writer() throw()
  {
    return ReferenceCounting::add_ref(this);
  }

  SimpleWriter_var
  BaseWriter::as_simple_writer() throw()
  {
    return SimpleWriter_var();
  }

  StructWriter_var
  BaseWriter::as_struct_writer() throw()
  {
    return StructWriter_var();
  }
}

