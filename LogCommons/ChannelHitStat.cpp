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


#include "ChannelHitStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
ChannelHitStatTraits::B::base_name_ = "ChannelHitStat";

template <> const char*
ChannelHitStatTraits::B::signature_ = "ChannelHitStat";

template <> const char*
ChannelHitStatTraits::B::current_version_ = "3.3";

std::istream&
operator>>(std::istream& is, ChannelHitStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelHitStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelHitStatInnerKey& key)
{
  is >> key.channel_id_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelHitStatInnerKey& key)
{
  os << key.channel_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelHitStatInnerData& data)
{
  is >> data.hits_;
  is >> data.hits_urls_;
  is >> data.hits_kws_;
  is >> data.hits_search_kws_;
  is >> data.hits_url_kws_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelHitStatInnerData& data)
{
  os << data.hits_ << '\t';
  os << data.hits_urls_ << '\t';
  os << data.hits_kws_ << '\t';
  os << data.hits_search_kws_ << '\t';
  os << data.hits_url_kws_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelHitStatInnerData_V_1_0& data
)
{
  is >> data.hits_;
  is >> data.hits_urls_;
  is >> data.hits_kws_;
  is >> data.hits_search_kws_;
  is >> data.hits_repeat_kws_;
  return is;
}

} // namespace LogProcessing
} // namespace AdServer

