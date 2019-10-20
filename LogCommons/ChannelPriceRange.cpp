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


#include "ChannelPriceRange.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ChannelPriceRangeTraits::B::base_name_ =
  "ChannelPriceRange";
template <> const char* ChannelPriceRangeTraits::B::signature_ =
  "ChannelPriceRange";
template <> const char* ChannelPriceRangeTraits::B::current_version_ =
  "3.0";

FixedBufStream<TabCategory>&
operator>>(
  FixedBufStream<TabCategory>& is,
  ChannelPriceRangeInnerKey& key)
  throw(eh::Exception)
{
  Aux_::StringIoWrapper creative_size_wrapper;
  is >> creative_size_wrapper;
  key.creative_size_ = creative_size_wrapper;
  Aux_::StringIoWrapper country_code_wrapper;
  is >> country_code_wrapper;

  if (country_code_wrapper == "-")
  {
    key.country_code_ = std::string();
  }
  else
  {
    key.country_code_ = std::move(country_code_wrapper);
  }

  is >> key.channel_id_;
  is >> key.ecpm_;
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelPriceRangeInnerKey& key)
  throw(eh::Exception)
{
  os << Aux_::StringIoWrapper(key.creative_size_.str()) << '\t';
  if (key.country_code_.empty())
  {
    os << "-\t";
  }
  else
  {
    os << Aux_::StringIoWrapper(key.country_code_.str()) << '\t';
  }
  os << key.channel_id_ << '\t';
  os << key.ecpm_ << '\t';
  os << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ChannelPriceRangeInnerData& data)
  throw(eh::Exception)
{
  is >> data.unique_users_count_;
  is >> data.impops_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ChannelPriceRangeInnerData& data)
  throw(eh::Exception)
{
  return os
    << data.unique_users_count_ << '\t'
    << data.impops_;
}

} // namespace LogProcessing
} // namespace AdServer

