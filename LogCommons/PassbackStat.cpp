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


#include "PassbackStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <>
const char* PassbackStatTraits::B::base_name_ = "PassbackStat";
template <>
const char* PassbackStatTraits::B::signature_ = "PassbackStat";
template <>
const char* PassbackStatTraits::B::current_version_ = "3.3";

std::istream&
operator>>(std::istream& is, PassbackStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const PassbackStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, PassbackStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.user_status_;
  is >> key.country_code_;
  is >> key.tag_id_;
  is >> key.size_id_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const PassbackStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.user_status_ << '\t';
  os << key.country_code_ << '\t';
  os << key.tag_id_ << '\t';
  os << key.size_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, PassbackStatInnerKey_V_2_4& key)
  throw(eh::Exception)
{
  is >> key.user_status_;
  is >> key.country_code_;
  is >> key.tag_id_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, PassbackStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.requests_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const PassbackStatInnerData& data)
  throw(eh::Exception)
{
  os << data.requests_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

