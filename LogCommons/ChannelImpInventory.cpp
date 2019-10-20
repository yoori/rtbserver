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


#include "ChannelImpInventory.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
ChannelImpInventoryTraits::B::base_name_ = "ChannelImpInventory";

template <> const char*
ChannelImpInventoryTraits::B::signature_ = "ChannelImpInventory";

template <> const char*
ChannelImpInventoryTraits::B::current_version_ = "1.0";

std::istream&
operator>>(std::istream& is, ChannelImpInventoryKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelImpInventoryKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

inline
FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelImpInventoryData::DataT::Counter& cntr
)
  throw(eh::Exception)
{
  is >> cntr.imps;
  is >> cntr.user_count;
  is >> cntr.value;
  return is;
}

inline
std::ostream&
operator<<(
  std::ostream& os,
  const ChannelImpInventoryData::DataT::Counter& cntr
)
  throw(eh::Exception)
{
  os << cntr.imps << '\t';
  os << cntr.user_count << '\t';
  os << cntr.value;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelImpInventoryInnerKey& key)
  throw(eh::Exception)
{
  is >> key.channel_id_;
  char ccg_type = '\0';
  is >> ccg_type;
  key.ccg_type_ = ChannelImpInventoryInnerKey::char_to_ccg_type_(ccg_type);

  if (is)
  {
    key.calc_hash_();
  }

  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelImpInventoryInnerKey& key)
  throw(eh::Exception)
{
  os << key.channel_id_ << '\t';
  os << key.ccg_type();
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelImpInventoryInnerData& data)
  throw(eh::Exception)
{
  TokenizerInputArchive<Aux_::NoInvariants> ia(is);
  ia >> data;
  data.meaningful_data_ = true;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelImpInventoryInnerData& data)
  throw(eh::Exception)
{
  SimpleTabOutputArchive oa(os);
  oa << data;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

