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

#include "ResearchLogs.hpp"

namespace AdServer
{
namespace LogProcessing
{
  template <> const char*
  ResearchBidTraits::B::base_name_ = "<UNDEFINED>";
  template <> const char*
  ResearchBidTraits::B::signature_ = "<UNDEFINED>";
  template <> const char*
  ResearchBidTraits::B::current_version_ = "<UNDEFINED>";

  template <> const char*
  ResearchImpressionTraits::B::base_name_ = "<UNDEFINED>";
  template <> const char*
  ResearchImpressionTraits::B::signature_ = "<UNDEFINED>";
  template <> const char*
  ResearchImpressionTraits::B::current_version_ = "<UNDEFINED>";

  template <> const char*
  ResearchClickTraits::B::base_name_ = "<UNDEFINED>";
  template <> const char*
  ResearchClickTraits::B::signature_ = "<UNDEFINED>";
  template <> const char*
  ResearchClickTraits::B::current_version_ = "<UNDEFINED>";

  template <> const char*
  ResearchActionTraits::B::base_name_ = "<UNDEFINED>";
  template <> const char*
  ResearchActionTraits::B::signature_ = "<UNDEFINED>";
  template <> const char*
  ResearchActionTraits::B::current_version_ = "<UNDEFINED>";
} // namespace LogProcessing
} // namespace AdServer

