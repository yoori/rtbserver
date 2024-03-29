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

#ifndef _CODE_TYPEDEFELEMENT_HPP_
#define _CODE_TYPEDEFELEMENT_HPP_

#include <Declaration/BaseType.hpp>
#include "Element.hpp"

namespace Code
{
  class TypeDefElement: public Element
  {
  public:
    TypeDefElement(
      const char* type_name_val,
      Declaration::BaseType* base_type_val)
      throw();

    const char* type_name() const throw();

    Declaration::BaseType_var base_type() const throw();

    virtual void visited(ElementVisitor* visitor) const throw();

  protected:
    virtual ~TypeDefElement() throw() {}
    
  private:
    std::string type_name_;
    Declaration::BaseType_var base_type_;
  };
}

namespace Code
{
  inline
  TypeDefElement::TypeDefElement(
    const char* type_name_val,
    Declaration::BaseType* base_type_val)
    throw()
    : type_name_(type_name_val),
      base_type_(ReferenceCounting::add_ref(base_type_val))
  {}

  inline
  const char*
  TypeDefElement::type_name() const throw()
  {
    return type_name_.c_str();
  }

  inline
  Declaration::BaseType_var
  TypeDefElement::base_type() const throw()
  {
    return base_type_;
  }
  
  inline
  void
  TypeDefElement::visited(ElementVisitor* visitor) const throw()
  {
    visitor->visit_i(this);
  }
}

#endif /*_CODE_TYPEDEFELEMENT_HPP_*/
