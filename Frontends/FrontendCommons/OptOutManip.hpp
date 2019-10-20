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


#ifndef OPT_OUT_MANIP_HPP
#define OPT_OUT_MANIP_HPP

#include <string>
#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <String/StringManip.hpp>

namespace AdServer
{
  class OptInDays
  {
    public:
    DECLARE_EXCEPTION(InvalidParam, eh::DescriptiveException);

    static
    void
    load_opt_in_days(const String::SubString& value, Generics::Time& val)
      throw(InvalidParam);

    static const char* save_opt_in_days(
      const Generics::Time& val,
      std::string& buf)
      throw();
  };
}

namespace AdServer
{
  inline
  void
  OptInDays::load_opt_in_days(const String::SubString& value,
    Generics::Time& val) throw (InvalidParam)
  {
    std::string decoded;
    String::StringManip::base64mod_decode(decoded, value);
    if(decoded.size() != Generics::Time::TIME_PACK_LEN)
    {
      throw InvalidParam(
        "FrontendCommons::load_opt_in_days(): "
        "noncorrect input buffer length.");
    }

    val.unpack(decoded.data());
  }

  inline
  const char* OptInDays::save_opt_in_days(
    const Generics::Time& val,
    std::string& buf)
    throw()
  {
    char tbuf[Generics::Time::TIME_PACK_LEN];
    val.pack(tbuf);
    String::StringManip::base64mod_encode(buf, tbuf, sizeof(tbuf));

    return buf.c_str();
  }
}

#endif//OPT_OUT_MANIP_HPP
