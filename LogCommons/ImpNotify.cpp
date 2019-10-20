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


#include "ImpNotify.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
ImpNotifyTraits::B::base_name_ = "ImpNotify";
template <> const char*
ImpNotifyTraits::B::signature_ = "ImpNotify";
template <> const char*
ImpNotifyTraits::B::current_version_ = "3.3";

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ImpNotifyData& data)
  throw(eh::Exception)
{
  is >> data.time;
  is >> data.user_id;
  is >> data.channels;
  is >> data.revenue;
  if (is)
  {
    data.invariant();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ImpNotifyData& data)
  throw(eh::Exception)
{
  data.invariant();
  os << data.time << '\t';
  os << data.user_id << '\t';
  os << data.channels << '\t';
  os << data.revenue;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

