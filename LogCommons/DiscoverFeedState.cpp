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


#include "DiscoverFeedState.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* DiscoverFeedStateTraits::B::base_name_ = "DiscoverFeedState";
template <> const char* DiscoverFeedStateTraits::B::signature_ = "DiscoverFeedState";
template <> const char* DiscoverFeedStateTraits::B::current_version_ = "1.0";

std::istream&
operator>>(std::istream &is, DiscoverFeedStateKey &key)
{
  is >> key.feed_id_;
  return is;
}

std::ostream&
operator<<(std::ostream &os, const DiscoverFeedStateKey &key)
  throw(eh::Exception)
{
  os << key.feed_id_;
  return os;
}

std::istream&
operator>>(std::istream &is, DiscoverFeedStateData &data)
{
  is >> data.time_;
  read_tab(is);
  is >> data.total_news_;
  return is;
}

std::ostream&
operator<<(std::ostream &os, const DiscoverFeedStateData &data)
{
  os << data.time_ << '\t';
  os << data.total_news_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

