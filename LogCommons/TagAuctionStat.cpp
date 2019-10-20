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

/// @file  TagAuctionStat.cpp

#include "TagAuctionStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* TagAuctionStatTraits::B::base_name_ =
  "TagAuctionStat";
template <> const char* TagAuctionStatTraits::B::signature_ =
  "TagAuctionStat";
template <> const char* TagAuctionStatTraits::B::current_version_ =
  "1.0"; // Last change: AdServer v2.3

std::istream&
operator>>(std::istream& is, TagAuctionStatKey& key)
  throw(eh::Exception)
{
  is >> key.pub_sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const TagAuctionStatKey& key)
  throw(eh::Exception)
{
  os << key.pub_sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, TagAuctionStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.tag_id_;
  is >> key.auction_ccg_count_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const TagAuctionStatInnerKey& key)
  throw(eh::Exception)
{
  os << key.tag_id_ << '\t';
  os << key.auction_ccg_count_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, TagAuctionStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.requests_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const TagAuctionStatInnerData& data)
  throw(eh::Exception)
{
  os << data.requests_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

