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

#ifndef _CODE_TYPEELEMENT_HPP_
#define _CODE_TYPEELEMENT_HPP_

#include <Declaration/BaseType.hpp>
#include "Element.hpp"

namespace Code
{
  class TypeElement: public Element
  {
  public:
    TypeElement(Declaration::BaseType* type_val) throw();

    Declaration::BaseType_var type() const throw();

    virtual void visited(ElementVisitor* visitor) const throw();

  private:
    Declaration::BaseType_var type_;
  };
}

namespace Code
{
  inline
  TypeElement::TypeElement(Declaration::BaseType* type_val)
    throw()
    : type_(ReferenceCounting::add_ref(type_val))
  {}

  inline
  Declaration::BaseType_var
  TypeElement::type() const throw()
  {
    return type_;
  }

  inline
  void
  TypeElement::visited(ElementVisitor* visitor) const throw()
  {
    visitor->visit_i(this);
  }
}

#endif /*_CODE_TYPEELEMENT_HPP_*/
