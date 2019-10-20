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

#include "WebwiseDiscoverTagStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* WebwiseDiscoverTagStatTraits::B::base_name_ = "WebwiseDiscoverTagStat";
template <> const char* WebwiseDiscoverTagStatTraits::B::signature_ = "WebwiseDiscoverTagStat";
template <> const char* WebwiseDiscoverTagStatTraits::B::current_version_ = "2.3";

std::istream&
operator>>(std::istream &is, WebwiseDiscoverTagStatKey &key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash();
  return is;
}

std::ostream&
operator<<(std::ostream &os, const WebwiseDiscoverTagStatKey &key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

std::istream&
operator>>(std::istream &is, WebwiseDiscoverTagStatInnerKey &key)
  throw(eh::Exception)
{
  is >> key.wdtag_id_;
  read_tab(is);
  is >> key.opted_in_;
  read_tab(is);
  is >> key.test_;
  key.calc_hash();
  return is;
}

std::ostream&
operator<<(std::ostream &os, const WebwiseDiscoverTagStatInnerKey &key)
  throw(eh::Exception)
{
  os << key.wdtag_id_ << '\t';
  os << key.opted_in_ << '\t';
  os << key.test_;
  return os;
}

std::istream&
operator>>(std::istream &is, WebwiseDiscoverTagStatInnerData &data)
  throw(eh::Exception)
{
  is >> data.imps_;
  read_tab(is);
  is >> data.clicks_;
  return is;
}

std::ostream&
operator<<(std::ostream &os, const WebwiseDiscoverTagStatInnerData &data)
  throw(eh::Exception)
{
  os << data.imps_ << '\t';
  os << data.clicks_;
  return os;
}

std::istream&
operator>>(std::istream &is, WebwiseDiscoverTagStatInnerData_V_1_1 &data)
  throw(eh::Exception)
{
  is >> data.imps_;
  return is;
}

} // namespace LogProcessing
} // namespace AdServer

