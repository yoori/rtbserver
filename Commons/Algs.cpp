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

#include "Algs.hpp"

namespace Algs
{
  DECLARE_EXCEPTION(InvalidHex, eh::DescriptiveException);

  unsigned long
  hex_encode(
    Generics::ArrayAutoPtr<unsigned char>& res,
    const String::SubString& src)
    throw(eh::Exception)
  {
    static const char* FUN = "hex_encode()";

    res.reset(src.size() / 2);
    unsigned char* res_it = res.get();
    for(String::SubString::ConstPointer it = src.begin();
        it != src.end(); ++it, ++res_it)
    {
      unsigned char high_val = static_cast<unsigned char>(
      String::AsciiStringManip::hex_to_int(*it++));
      if(it == src.end())
      {
        Stream::Error ostr;
        ostr << FUN << ": unexpected end of value";
        throw InvalidHex(ostr);
      }

      *res_it = (high_val << 4) +
        static_cast<unsigned char>(
          String::AsciiStringManip::hex_to_int(*it));
    }

    return src.size() / 2;
  }
}
