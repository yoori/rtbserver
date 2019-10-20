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


#include "ChannelTriggerImpStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
ChannelTriggerImpStatTraits::B::base_name_ = "ChannelTriggerImpStat";

template <> const char*
ChannelTriggerImpStatTraits::B::signature_ = "ChannelTriggerImpStat";

template <> const char*
ChannelTriggerImpStatTraits::B::current_version_ = "3.5";

std::istream&
operator>>(std::istream& is, ChannelTriggerImpStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelTriggerImpStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelTriggerImpStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.channel_trigger_id_;
  is >> key.channel_id_;
  is >> key.type_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelTriggerImpStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.channel_trigger_id_ << '\t';
  os << key.channel_id_ << '\t';
  os << key.type_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelTriggerImpStatInnerKey_V_2_3& key
)
  throw(eh::Exception)
{
  is >> key.channel_trigger_id_;
  is >> key.type_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelTriggerImpStatInnerData& data
)
  throw(eh::Exception)
{
  is >> data.approximated_imps_;
  is >> data.approximated_clicks_;
  if (is)
  {
    data.invariant();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelTriggerImpStatInnerData& data)
  throw(eh::Exception)
{
  data.invariant();
  os << data.approximated_imps_ << '\t';
  os << data.approximated_clicks_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

