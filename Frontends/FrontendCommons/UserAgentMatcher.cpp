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


#include <sstream>
#include <fstream>

#include "UserAgentMatcher.hpp"
#include <Stream/MemoryStream.hpp>

namespace FrontendCommons
{
  void UserAgentMatcher::init(const char *filename)
    throw(Exception, eh::Exception)
  {
    hash_set_.clear();
    std::ifstream ifs(filename);
    if (!ifs)
    {
      Stream::Error ostr;
      ostr << __PRETTY_FUNCTION__ << ": failed to open file "
        << (filename ? filename : "<filename is NULL>");
      throw Exception(ostr);
    }
    std::string user_agent;
    while (std::getline(ifs, user_agent))
    {
      hash_set_.insert(user_agent);
    }
  }
} // namespace FrontendCommons

