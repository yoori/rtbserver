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

#ifndef _CODE_INCLUDEELEMENT_HPP_
#define _CODE_INCLUDEELEMENT_HPP_

#include "Element.hpp"

namespace Code
{
  class IncludeElement: public Element
  {
  public:
    IncludeElement(const char* file_val) throw();

    const char* file() const throw();

    virtual void visited(ElementVisitor* visitor) const throw();

  protected:
    virtual ~IncludeElement() throw() {}
    
  private:
    std::string file_;
  };
}

namespace Code
{
  inline
  IncludeElement::IncludeElement(const char* file_val) throw()
    : file_(file_val)
  {}

  inline
  const char*
  IncludeElement::file() const throw()
  {
    return file_.c_str();
  }
  
  inline
  void
  IncludeElement::visited(ElementVisitor* visitor) const throw()
  {
    visitor->visit_i(this);
  }
}

#endif /*_CODE_INCLUDEELEMENT_HPP_*/
