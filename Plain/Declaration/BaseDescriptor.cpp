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

#include "BaseDescriptor.hpp"
#include "SimpleDescriptor.hpp"
#include "StructDescriptor.hpp"
#include "CompleteTemplateDescriptor.hpp"

namespace Declaration
{
  BaseDescriptor::BaseDescriptor(const char* name_val)
    : BaseType(name_val)
  {}

  BaseDescriptor_var
  BaseDescriptor::as_descriptor() throw()
  {
    return ReferenceCounting::add_ref(this);
  }

  SimpleDescriptor_var
  BaseDescriptor::as_simple() throw()
  {
    return SimpleDescriptor_var();
  }

  StructDescriptor_var
  BaseDescriptor::as_struct() throw()
  {
    return StructDescriptor_var();
  }

  CompleteTemplateDescriptor_var
  BaseDescriptor::as_complete_template() throw()
  {
    return CompleteTemplateDescriptor_var();
  }
}
