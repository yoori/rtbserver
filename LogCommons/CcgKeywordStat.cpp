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


#include "CcgKeywordStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* CcgKeywordStatTraits::B::base_name_ = "CCGKeywordStat";
template <> const char* CcgKeywordStatTraits::B::signature_ = "CCGKeywordStat";
template <> const char* CcgKeywordStatTraits::B::current_version_ = "2.3";

std::istream&
operator>>(std::istream& is, CcgKeywordStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgKeywordStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, CcgKeywordStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.ccg_keyword_id_;
  is >> key.currency_exchange_id_;
  is >> key.cc_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgKeywordStatInnerKey& key)
  throw(eh::Exception)
{
  os << key.ccg_keyword_id_ << '\t';
  os << key.currency_exchange_id_ << '\t';
  os << key.cc_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, CcgKeywordStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.imps_;
  is >> data.clicks_;
  is >> data.adv_revenue_;
  is >> data.adv_comm_revenue_;
  is >> data.pub_advcurrency_amount_;
  if (is)
  {
    data.invariant();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgKeywordStatInnerData& data)
  throw(eh::Exception)
{
  data.invariant();
  os << data.imps_ << '\t';
  os << data.clicks_ << '\t';
  os << data.adv_revenue_ << '\t';
  os << data.adv_comm_revenue_ << '\t';
  os << data.pub_advcurrency_amount_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  CcgKeywordStatInnerData_V_1_4& data
)
  throw(eh::Exception)
{
  is >> data.imps_;
  is >> data.clicks_;
  is >> data.revenue_;
  if (is)
  {
    data.invariant();
  }
  return is;
}

} // namespace LogProcessing
} // namespace AdServer

