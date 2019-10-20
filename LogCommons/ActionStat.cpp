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


#include "ActionStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ActionStatTraits::B::base_name_ =
  "ActionStat";
template <> const char* ActionStatTraits::B::signature_ =
  "ActionStat";
template <> const char* ActionStatTraits::B::current_version_ =
  "3.7";

namespace v3_2_0_28
{
  std::istream&
  operator>>(std::istream& is, ActionStatKey& key)
  {
    is >> key.sdate_;
    read_eol(is);
    is >> key.colo_id_;
    key.calc_hash_();
    return is;
  }

  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ActionStatInnerKey& key)
  {
    is >> key.action_request_id_;
    is >> key.request_id_;
    is >> key.cc_id_;
    key.calc_hash_();
    return is;
  }

  FixedBufStream<TabCategory>&
  operator>>(FixedBufStream<TabCategory>& is, ActionStatInnerData& data)
    throw(eh::Exception)
  {
    data.holder_ = new ActionStatInnerData::DataHolder();

    is >> data.holder_->action_id;
    is >> data.holder_->tag_id;
    is >> data.holder_->order_id;
    is >> data.holder_->country_code;
    is >> data.holder_->referrer;
    is >> data.holder_->imp_date;
    is >> data.holder_->click_date;
    is >> data.holder_->cur_value;
    return is;
  }
}

std::istream&
operator>>(std::istream& is, ActionStatKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ActionStatKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ActionStatInnerKey& key)
{
  is >> key.action_request_id_;
  is >> key.request_id_;
  is >> key.cc_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ActionStatInnerKey& key)
{
  os << key.action_request_id_ << '\t';
  os << key.request_id_ << '\t';
  os << key.cc_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ActionStatInnerData& data)
  throw(eh::Exception)
{
  data.holder_ = new ActionStatInnerData::DataHolder();

  is >> data.holder_->action_id;
  is >> data.holder_->tag_id;
  is >> data.holder_->order_id;
  is >> data.holder_->country_code;
  is >> data.holder_->referrer;
  is >> data.holder_->imp_date;
  is >> data.holder_->click_date;
  is >> data.holder_->cur_value;
  is >> data.holder_->device_channel_id;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ActionStatInnerData& data)
  throw(eh::Exception)
{
  os << data.holder_->action_id << '\t';
  os << data.holder_->tag_id << '\t';
  os << data.holder_->order_id << '\t';
  os << data.holder_->country_code << '\t';
  os << data.holder_->referrer << '\t';
  os << data.holder_->imp_date << '\t';
  os << data.holder_->click_date << '\t';
  os << data.holder_->cur_value<< '\t';
  os << data.holder_->device_channel_id;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

