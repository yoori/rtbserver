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

#include "Request_Base.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

FixedBufStream<CommaCategory>&
operator>>(FixedBufStream<CommaCategory>& is, UserProperty& property) throw(eh::Exception)
{
  String::SubString token = is.read_token();

  if (token.empty())
  {
    is.setstate(std::ios_base::eofbit);
    return is;
  }

  // 'T' = not allowed
  // if it equal to "T=" (and allowed) remove next block.
  {
    static const EqlCategory eq;
    const char* res = eq.find_owned(token.begin(), token.end());
    if (res == token.end() || !res)
    {
      is.setstate(std::ios_base::failbit);
      property.first.clear();
      property.second.clear();

      return is;
    }
  }

  String::StringManip::Splitter<EqlCategory, true> splitter(token);
  String::SubString tmp;
  size_t count = 0;
  while(splitter.get_token(tmp))
  {
    String::StringManip::trim(tmp);

    // empty property.first not allowed ('=V')
    if (count == 0 )
    {
      if (tmp.empty())
      {
        is.setstate(std::ios_base::failbit);
        property.first.clear();
        property.second.clear();
        break;
      }
      // any category, read only one value
      FixedBufStream<EqlCategory> fbs(tmp);
      fbs >> property.first;
    }

    if (count == 1)
    {
      if (!tmp.empty())
      {
        FixedBufStream<EqlCategory> fbs(tmp);
        fbs >> property.second;
      }
      else
      {
        property.second.clear();
      }
    }

    if (++count > 2)
    {
      break;
    }
  }

  // empty property.second allowed ('N=')
  if (count == 1)
  {
    property.second.clear();
  }
  // smth like 'p1=p2=p3' - unsupported format
  if (count > 2)
  {
    is.setstate(std::ios_base::failbit);
  }

  return is;
}

} // namespace LogProcessing
} // namespace AdServer
