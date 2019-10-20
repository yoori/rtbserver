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

#include <tests/AutoTests/Commons/Common.hpp>

std::string
tolower(size_t size, const char* str)
{
  std::string ret;
  ret.reserve(size);
  for(const char* i = str; *i != 0; ++i)
  {
    ret += ::tolower(*i);
  }
  return ret;
}

std::string
toupper(size_t size, const char* str)
{
  std::string ret;
  ret.reserve(size);
  for(const char* i = str; *i != 0; ++i)
  {
    ret += ::toupper(*i);
  }
  return ret;
}

namespace AutoTest
{
  std::string time_to_gmt_str( time_t time )
  {
    return Generics::Time(time).get_gm_time().format("%d-%m-%Y:%H-%M-%S");
  }

  time_t gmt_str_to_time( const char* gmt_str ) throw (eh::Exception)
  {
    return Generics::Time(String::SubString(gmt_str),
      "%d-%m-%Y:%H-%M-%S").tv_sec;
  }
  
  Comparable::~Comparable() throw ()
  {
  }

  bool
  ComparableRegExp::compare(const char* other) const
  {
    try
    {
      String::RegEx re(
        std::string("\\A") + value_ + "\\Z",
        PCRE_MULTILINE | PCRE_DOTALL);
      return re.match( String::SubString(other));
    }
    catch (const eh::Exception&)
    {
      return value_ == other;
    }
  }

  bool
  precisely_number::compare(const char* other) const
  {
    precisely_number other_number(other, precisely_);
    return *this == other_number;
  }

  std::string
  precisely_number::str() const
  {
    std::ostringstream ost;
    ost << *this;
    return ost.str();
  }

  StringList
  parse_list (const char* str)
  {
    StringList ret;
    String::StringManip::Tokenizer tokenizer(String::SubString(str), ",");
    String::SubString token;
    while (tokenizer.get_token(token))
    {
      String::StringManip::trim(token);
      ret.push_back(token.str());
    }
    return ret;
  }
}
