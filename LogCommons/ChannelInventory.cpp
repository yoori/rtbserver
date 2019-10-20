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


#include "ChannelInventory.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
ChannelInventoryTraits::B::base_name_ = "ChannelInventory";

template <> const char*
ChannelInventoryTraits::B::signature_ = "ChannelInventory";

template <> const char*
ChannelInventoryTraits::B::current_version_ = "1.3";

std::istream&
operator>>(std::istream& is, ChannelInventoryKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelInventoryKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelInventoryInnerKey& key)
  throw(eh::Exception)
{
  is >> key.channel_id_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelInventoryInnerKey& key)
  throw(eh::Exception)
{
  os << key.channel_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelInventoryInnerData& data)
  throw(eh::Exception)
{
  is >> data.sum_user_ecpm_;
  is >> data.active_user_count_;
  is >> data.total_user_count_;
  data.meaningful_data_ = true;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelInventoryInnerData& data)
  throw(eh::Exception)
{
  os << data.sum_user_ecpm_ << '\t';
  os << data.active_user_count_ << '\t';
  os << data.total_user_count_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

