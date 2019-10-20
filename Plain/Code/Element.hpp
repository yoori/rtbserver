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

#ifndef _CODE_ELEMENT_HPP_
#define _CODE_ELEMENT_HPP_

#include <list>
#include <iostream>
#include <eh/Exception.hpp>
#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

namespace Code
{
  class Element;
  class IncludeElement;
  class NamespaceElement;
  class TypeDefElement;
  class TypeElement;

  class ElementVisitor
  {
  public:
    virtual ~ElementVisitor() throw() {}

    virtual void visit(const Element*) throw();

    virtual void visit_i(const Element*) throw() = 0;

    virtual void visit_i(const IncludeElement*) throw();

    virtual void visit_i(const NamespaceElement*) throw();

    virtual void visit_i(const TypeDefElement*) throw();

    virtual void visit_i(const TypeElement*) throw();
  };
  
  class Element: public ReferenceCounting::DefaultImpl<>
  {
  public:
    virtual void visited(ElementVisitor* visitor) const throw();
    
  protected:
    virtual ~Element() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<Element> Element_var;

  struct ElementList:
    public std::list<Element_var>,
    public ReferenceCounting::DefaultImpl<>
  {
  protected:
    virtual ~ElementList() throw() {}
  };
  
  typedef ReferenceCounting::SmartPtr<ElementList>
    ElementList_var;
}

namespace Code
{
  inline
  void ElementVisitor::visit(const Element* elem) throw()
  {
    elem->visited(this);
  }
}

#endif /*_CODE_ELEMENT_HPP_*/
