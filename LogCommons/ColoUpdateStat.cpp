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


#include "ColoUpdateStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ColoUpdateStatTraits::B::base_name_ = "ColoUpdateStat";
template <> const char* ColoUpdateStatTraits::B::signature_ = "ColoUpdateStat";
template <> const char* ColoUpdateStatTraits::B::current_version_ = "1.0";

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ColoUpdateStatData::Version& version
)
  throw(eh::Exception)
{
  is >> static_cast<StringIoWrapperOptional&>(version);
  version.parse_();
  return is;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ColoUpdateStatKey& key)
{
  is >> key.colo_id_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ColoUpdateStatKey& key)
  throw(eh::Exception)
{
  os << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ColoUpdateStatData& data)
{
  is >> data.last_channels_update_;
  is >> data.last_campaigns_update_;
  is >> data.version_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ColoUpdateStatData& data)
{
  os << data.last_channels_update_ << '\t';
  os << data.last_campaigns_update_ << '\t';
  os << data.version_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

