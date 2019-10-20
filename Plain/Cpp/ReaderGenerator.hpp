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

#ifndef PLAIN_CPP_READERGENERATOR_HPP
#define PLAIN_CPP_READERGENERATOR_HPP

#include <string>
#include <iostream>

#include <Declaration/SimpleDescriptor.hpp>
#include <Declaration/SimpleReader.hpp>
#include <Declaration/StructDescriptor.hpp>
#include <Declaration/StructReader.hpp>
#include <Declaration/CompleteTemplateDescriptor.hpp>

namespace Cpp
{
  class ReaderGenerator
  {
  public:
    ReaderGenerator(std::ostream& out, const char* offset)
      throw();

    void generate_decl(
      Declaration::StructReader* struct_reader)
      throw();

    void generate_impl(
      Declaration::StructReader* struct_reader)
      throw();

  private:
    void generate_ctor_impl_(
      const Declaration::StructReader* struct_reader)
      throw();

    void generate_field_funs_impl_(
      const Declaration::StructReader* struct_reader)
      throw();

  private:
    std::ostream& out_;
    std::string offset_;
  };
}

#endif /*PLAIN_CPP_READERGENERATOR_HPP*/
