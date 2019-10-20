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


#include <LogCommons/LogCommons.hpp>
#include "ConvLogger.hpp"

namespace AdServer
{
namespace LogProcessing
{
  template <> const char*
  ConvTraits::B::base_name_ = "Conv";

  template <> const char*
  ConvTraits::B::signature_ = "Conv";

  template <> const char*
  ConvTraits::B::current_version_ = "3.6";

  FixedBufStream<TabCategory>&
    operator>>(FixedBufStream<TabCategory>& is,
      ConvData& data)
    throw(eh::Exception)
  {
    is >> data.time_;
    is >> data.user_id_;
    is >> data.action_id_;
    return is;
  }

  std::ostream&
  operator<<(std::ostream& os, const ConvData& data)
    throw(eh::Exception)
  {
    os << data.time_ << '\t';
    os << data.user_id_ << '\t';
    os << data.action_id_ << '\t';
    return os;
  }
} // namespace LogProcessing
} // namespace AdServer

