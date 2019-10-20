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


#include "ColoUsers.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ColoUsersTraits::B::base_name_ = "ColoUsers";
template <> const char* ColoUsersTraits::B::signature_ = "ColoUsers";
template <> const char* ColoUsersTraits::B::current_version_ = "1.2";

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ColoUsersKey& key)
{
  is >> key.sdate_;
  is >> key.isp_sdate_;
  is >> key.colo_id_;
  is >> key.created_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ColoUsersKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.sdate_ << '\t';
  os << key.isp_sdate_ << '\t';
  os << key.colo_id_ << '\t';
  os << key.created_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ColoUsersData& data)
{
  is >> data.users_count_;
  is >> data.weekly_users_count_;
  is >> data.monthly_users_count_;
  is >> data.daily_network_users_count_;
  is >> data.monthly_network_users_count_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ColoUsersData& data)
{
  os << data.users_count_ << '\t';
  os << data.weekly_users_count_ << '\t';
  os << data.monthly_users_count_ << '\t';
  os << data.daily_network_users_count_ << '\t';
  os << data.monthly_network_users_count_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

