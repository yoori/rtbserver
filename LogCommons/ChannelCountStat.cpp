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


#include "ChannelCountStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ChannelCountStatTraits::B::base_name_ = "ChannelCountStat";
template <> const char* ChannelCountStatTraits::B::signature_ = "ChannelCountStat";
template <> const char* ChannelCountStatTraits::B::current_version_ = "1.0";

std::istream&
operator>>(std::istream& is, ChannelCountStatKey& key)
{
  is >> key.isp_sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelCountStatKey& key)
  throw(eh::Exception)
{
  os << key.isp_sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelCountStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.channel_type_;
  is >> key.channel_count_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelCountStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.channel_type_ << '\t';
  os << key.channel_count_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelCountStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.users_count_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelCountStatInnerData& data)
  throw(eh::Exception)
{
  os << data.users_count_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

