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


#include "FailContext.hpp"
#include <Commons/PathManip.hpp>

namespace AutoTest
{
  void fail_message(
    Stream::Error& ostr,
    const String::SubString& description,
    const char* file_path,
    const char* function_name,
    const unsigned int line,
    const char* notes)
    throw ()
  {
    std::string file_name;
    AdServer::PathManip::split_path(file_path, 0, &file_name);

    ostr << description << std::endl <<
      "  " << file_name << ":" << line << ": " << function_name << "()";
    if(notes[0])
    {
      ostr << ": " << notes;
    }
  }
  
  void fail(
    const String::SubString& description,
    const char* file_path,
    const char* function_name,
    const unsigned int line,
    const char* notes)
    throw (Exception)
  {
    Stream::Error ostr;
    fail_message(ostr, description, file_path, function_name, line, notes);
    throw Exception(ostr);
  }

}

