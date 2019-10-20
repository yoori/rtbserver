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

#include "WebStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* WebStatTraits::B::base_name_ = "WebStat";
template <> const char* WebStatTraits::B::signature_ = "WebStat";
template <> const char* WebStatTraits::B::current_version_ = "3.5";

std::istream&
operator>>(std::istream& is, WebStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  if (is)
  {
    key.invariant_();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const WebStatKey& key)
  throw(eh::Exception)
{
  key.invariant_();
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, WebStatInnerKey_V_3_1& key)
  throw(eh::Exception)
{
  is >> key.ct_;
  is >> key.curct_;
  is >> key.browser_;
  is >> key.os_;
  is >> key.web_operation_id_;
  is >> key.result_;
  is >> key.user_status_;
  is >> key.test_;
  is >> key.tag_id_;
  is >> key.cc_id_;
  if (is)
  {
    key.invariant_();
    key.calc_hash_();
  }
  return is;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, WebStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.ct_;
  is >> key.curct_;
  is >> key.browser_;
  is >> key.os_;
  is >> key.source_;
  is >> key.web_operation_id_;
  is >> key.result_;
  is >> key.user_status_;
  is >> key.test_;
  is >> key.tag_id_;
  is >> key.cc_id_;
  if (is)
  {
    key.invariant_();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const WebStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant_();
  os << key.ct_ << '\t';
  os << key.curct_ << '\t';
  os << key.browser_ << '\t';
  os << key.os_ << '\t';
  os << key.source_ << '\t';
  os << key.web_operation_id_ << '\t';
  os << key.result_ << '\t';
  os << key.user_status_ << '\t';
  os << key.test_ << '\t';
  os << key.tag_id_ << '\t';
  os << key.cc_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, WebStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.count_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const WebStatInnerData& data)
  throw(eh::Exception)
{
  os << data.count_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

