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


#include "SearchTermStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
SearchTermStatTraits::B::base_name_ = "SearchTermStat";

template <> const char*
SearchTermStatTraits::B::signature_ = "SearchTermStat";

template <> const char*
SearchTermStatTraits::B::current_version_ = "2.5";

std::istream&
operator>>(std::istream& is, SearchTermStatKey& key)
  throw(eh::Exception)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const SearchTermStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, SearchTermStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.search_term_;
  key.invariant_();
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const SearchTermStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant_();
  os << key.search_term_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, SearchTermStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.hits_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const SearchTermStatInnerData& data)
  throw(eh::Exception)
{
  os << data.hits_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

