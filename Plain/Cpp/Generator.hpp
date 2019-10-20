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

#ifndef PLAIN_CPP_GENERATOR_HPP
#define PLAIN_CPP_GENERATOR_HPP

#include <Declaration/SimpleDescriptor.hpp>
#include <Declaration/SimpleReader.hpp>
#include <Declaration/SimpleWriter.hpp>
#include <Declaration/StructDescriptor.hpp>
#include <Declaration/StructReader.hpp>
#include <Declaration/StructWriter.hpp>

#include <Code/Element.hpp>
#include <Code/IncludeElement.hpp>
#include <Code/TypeElement.hpp>
#include <Code/TypeDefElement.hpp>
#include <Code/NamespaceElement.hpp>

namespace Cpp
{
  class Generator: public ReferenceCounting::DefaultImpl<>
  {
  public:
    void generate(
      std::ostream& out,
      std::ostream& out_inl_impl,
      std::ostream& out_cpp,
      Code::ElementList* elements) throw();

  protected:
    virtual ~Generator() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<Generator>
    Generator_var;
}

#endif /*PLAIN_CPP_GENERATOR_HPP*/
