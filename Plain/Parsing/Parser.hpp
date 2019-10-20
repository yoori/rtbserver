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

#ifndef PLAIN_PARSER_HPP
#define PLAIN_PARSER_HPP

#include <iostream>
#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Code/Element.hpp>
#include <Declaration/Namespace.hpp>

namespace Parsing
{
  class Parser:
    public ReferenceCounting::DefaultImpl<>
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    bool parse(
      std::ostream& error_ostr,
      Code::ElementList* elements,
      std::istream& istr,
      Declaration::Namespace_var* root_namespace = 0)
      throw(Exception);
  };

  typedef ReferenceCounting::SmartPtr<Parser> Parser_var;
}

#endif /*PLAIN_PARSER_HPP*/
