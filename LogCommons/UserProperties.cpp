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

#include "UserProperties.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* UserPropertiesTraits::B::base_name_ = "UserProperties";
template <> const char* UserPropertiesTraits::B::signature_ = "UserProperties";
template <> const char* UserPropertiesTraits::B::current_version_ = "1.2";

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, UserPropertiesKey& key)
{
  is >> key.sdate_;
  is >> key.isp_sdate_;
  is >> key.colo_id_;
  is >> key.user_status_;
  Aux_::StringIoWrapper property_name_wrapper;
  is >> property_name_wrapper;
  key.property_name_ = property_name_wrapper;
  Aux_::StringIoWrapper property_value_wrapper;
  is >> property_value_wrapper;
  key.property_value_ = property_value_wrapper;
  if (key.property_value_ == "-")
  {
    key.property_value_.clear();
  }
  key.invariant();
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const UserPropertiesKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.sdate_ << '\t';
  os << key.isp_sdate_ << '\t';
  os << key.colo_id_ << '\t';
  os << key.user_status_ << '\t';
  os << Aux_::StringIoWrapper(key.property_name_.text()) << '\t';
  if (key.property_value_.empty())
  {
    os << '-';
  }
  else
  {
    os << Aux_::StringIoWrapper(key.property_value_);
  }
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, UserPropertiesData& data)
{
  is >> data.profiling_requests_;
  is >> data.requests_;
  is >> data.imps_unverified_;
  is >> data.imps_verified_;
  is >> data.clicks_;
  is >> data.actions_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const UserPropertiesData& data)
{
  os << data.profiling_requests_ << '\t';
  os << data.requests_ << '\t';
  os << data.imps_unverified_ << '\t';
  os << data.imps_verified_ << '\t';
  os << data.clicks_ << '\t';
  os << data.actions_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

