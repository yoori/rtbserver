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


#include "CcgSelectionFailureStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* CcgSelectionFailureStatTraits::B::base_name_ =
  "CCGSelectionFailureStat";
template <> const char* CcgSelectionFailureStatTraits::B::signature_ =
  "CCGSelectionFailureStat";
template <> const char* CcgSelectionFailureStatTraits::B::current_version_ =
  "3.1";

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  CcgSelectionFailureStatInnerKey& key
)
  throw(eh::Exception)
{
  is >> key.ccg_id_;
  is >> key.combination_mask_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgSelectionFailureStatInnerKey& key)
  throw(eh::Exception)
{
  os << key.ccg_id_ << '\t';
  os << key.combination_mask_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  CcgSelectionFailureStatInnerData& data
)
  throw(eh::Exception)
{
  is >> data.requests_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const CcgSelectionFailureStatInnerData& data)
  throw(eh::Exception)
{
  os << data.requests_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

