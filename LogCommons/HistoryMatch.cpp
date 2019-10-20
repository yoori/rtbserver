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


#include "HistoryMatch.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* HistoryMatchTraits::B::base_name_ = "HistoryMatch";
template <> const char* HistoryMatchTraits::B::signature_ = "HistoryMatch";
template <> const char* HistoryMatchTraits::B::current_version_ = "1.0";

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is,
  HistoryMatchData& data) throw (eh::Exception)
{
  is >> data.time_;
  is >> data.user_id_;
  is >> data.temporary_;
  is >> data.last_colo_id_;
  is >> data.placement_colo_id_;
  is >> data.request_colo_id_;
  is >> data.search_channels_;
  is >> data.page_channels_;
  is >> data.url_channels_;
  return is;
}

std::ostream&
operator<<(std::ostream& os,
  const HistoryMatchData& data) throw (eh::Exception)
{
  return os
    << data.time_ << '\t'
    << data.user_id_ << '\t'
    << data.temporary_ << '\t'
    << data.last_colo_id_ << '\t'
    << data.placement_colo_id_ << '\t'
    << data.request_colo_id_ << '\t'
    << data.search_channels_ << '\t'
    << data.page_channels_ << '\t'
    << data.url_channels_;
}

} // namespace LogProcessing
} // namespace AdServer

