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

#include "Processor.hpp"
#include "YYParserAdapter.hpp"
#include <Parsing/YYParser.yy.hpp>
#include "Parser.hpp"

namespace Parsing
{
  bool Parser::parse(
    std::ostream& error_ostr,
    Code::ElementList* elements,
    std::istream& istr,
    Declaration::Namespace_var* root_namespace)
    throw(Exception)
  {
    Parsing::Processor_var processor = new Parsing::Processor(elements);

    YYParserAdapter parse_adapter;
    parse_adapter.processor = processor;
    parse_adapter.scanner = new yy::PlainScanner(&istr, &error_ostr);
    yy::PlainParser parser(parse_adapter);
    if(!parser.parse())
    {
      if(root_namespace)
      {
        *root_namespace = processor->root_namespace();
      }

      return true;
    }

    return false;
  }
}
