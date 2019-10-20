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


#include "TagPositionStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* TagPositionStatTraits::B::base_name_ =
  "TagPositionStat";
template <> const char* TagPositionStatTraits::B::signature_ =
  "TagPositionStat";
template <> const char* TagPositionStatTraits::B::current_version_ =
  "2.8";

std::istream&
operator>>(std::istream& is, TagPositionStatKey& key)
  throw(eh::Exception)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const TagPositionStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, TagPositionStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.tag_id_;
  is >> key.top_offset_;
  is >> key.left_offset_;
  is >> key.visibility_;
  is >> key.test_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const TagPositionStatInnerKey& key)
  throw(eh::Exception)
{
  os << key.tag_id_ << '\t';
  os << key.top_offset_ << '\t';
  os << key.left_offset_ << '\t';
  os << key.visibility_ << '\t';
  os << key.test_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, TagPositionStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.requests_;
  is >> data.imps_;
  is >> data.clicks_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const TagPositionStatInnerData& data)
  throw(eh::Exception)
{
  os << data.requests_ << '\t';
  os << data.imps_ << '\t';
  os << data.clicks_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, TagPositionStatInnerKey_V_2_7& key)
  throw(eh::Exception)
{
  is >> key.tag_id_;
  is >> key.top_offset_;
  is >> key.left_offset_;
  is >> key.visibility_;
  key.calc_hash_();
  return is;
}

} // namespace LogProcessing
} // namespace AdServer

