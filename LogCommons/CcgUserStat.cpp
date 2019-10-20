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


#include "CcgUserStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* CcgUserStatTraits::B::base_name_ = "CCGUserStat";
template <> const char* CcgUserStatTraits::B::signature_ = "CCGUserStat";
template <> const char* CcgUserStatTraits::B::current_version_ = "2.5";

std::istream&
operator>>(std::istream& is, CcgUserStatKey& key)
{
  is >> key.adv_sdate_;
  read_eol(is);
  is >> key.colo_id_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgUserStatKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.adv_sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, CcgUserStatInnerKey& key)
  throw(eh::Exception)
{
  is >> key.ccg_id_;
  is >> key.last_appearance_date_;
  if (is)
  {
    key.invariant();
    key.calc_hash_();
  }
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgUserStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << key.ccg_id_ << '\t';
  os << key.last_appearance_date_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, CcgUserStatInnerData& data)
  throw(eh::Exception)
{
  is >> data.unique_users_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgUserStatInnerData& data)
  throw(eh::Exception)
{
  os << data.unique_users_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

