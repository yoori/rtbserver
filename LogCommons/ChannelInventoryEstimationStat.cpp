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


#include "ChannelInventoryEstimationStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <>
const char* ChannelInventoryEstimationStatTraits::B::base_name_ =
  "ChannelInventoryEstimationStat";
template <>
const char* ChannelInventoryEstimationStatTraits::B::signature_ =
  "ChannelInventoryEstimationStat";
template <>
const char* ChannelInventoryEstimationStatTraits::B::current_version_ =
  "1.1"; // Last change: AdServer v2.2

const ChannelInventoryEstimationStatInnerKey::LevelT
  ChannelInventoryEstimationStatInnerKey::max_level_value_("2.0");

std::istream&
operator>>(std::istream& is, ChannelInventoryEstimationStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelInventoryEstimationStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelInventoryEstimationStatInnerKey& key
)
{
  is >> key.channel_id_;
  is >> key.level_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelInventoryEstimationStatInnerKey& key)
{
  key.invariant();
  os << key.channel_id_ << '\t';
  os << key.level_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelInventoryEstimationStatInnerData& data
)
{
  is >> data.users_regular_;
  is >> data.users_from_now_;
  return is;
}

std::ostream&
operator<<(
  std::ostream& os,
  const ChannelInventoryEstimationStatInnerData& data
)
{
  os << data.users_regular_ << '\t';
  os << data.users_from_now_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

