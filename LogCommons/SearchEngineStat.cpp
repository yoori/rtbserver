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


#include "SearchEngineStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char*
SearchEngineStatTraits::B::base_name_ = "SearchEngineStat";

template <> const char*
SearchEngineStatTraits::B::signature_ = "SearchEngineStat";

template <> const char* SearchEngineStatTraits::B::current_version_ = "2.5";

std::istream&
operator>>(std::istream& is, SearchEngineStatKey& key)
  throw(eh::Exception)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const SearchEngineStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, SearchEngineStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.search_engine_id_;
  is >> key.host_name_;
  key.invariant_();
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const SearchEngineStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant_();
  os << key.search_engine_id_ << '\t';
  os << key.host_name_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, SearchEngineStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.hits_;
  is >> data.hits_empty_page_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const SearchEngineStatInnerData& data)
  throw(eh::Exception)
{
  os << data.hits_ << '\t';
  os << data.hits_empty_page_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

