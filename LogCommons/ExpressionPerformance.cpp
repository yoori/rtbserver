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


#include "ExpressionPerformance.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ExpressionPerformanceTraits::B::base_name_ =
  "ExpressionPerformance";
template <> const char* ExpressionPerformanceTraits::B::signature_ =
  "ExpressionPerformance";
template <> const char* ExpressionPerformanceTraits::B::current_version_ =
  "1.1";

std::istream&
operator>>(std::istream& is, ExpressionPerformanceKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ExpressionPerformanceKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ExpressionPerformanceInnerKey& key)
{
  is >> key.cc_id_;
  Aux_::StringIoWrapper expression_wrapper;
  is >> expression_wrapper;
  key.expression_ = expression_wrapper;
  key.invariant();
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ExpressionPerformanceInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.cc_id_ << '\t';
  os << Aux_::StringIoWrapper(key.expression_);
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ExpressionPerformanceInnerData& data
)
{
  is >> data.imps_verified_;
  is >> data.clicks_;
  is >> data.actions_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ExpressionPerformanceInnerData& data)
{
  os << data.imps_verified_ << '\t';
  os << data.clicks_ << '\t';
  os << data.actions_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

