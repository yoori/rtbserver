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


#include "ActionRequest.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* ActionRequestTraits::B::base_name_ =
  "ActionRequest";
template <> const char* ActionRequestTraits::B::signature_ =
  "ActionRequest";
template <> const char* ActionRequestTraits::B::current_version_ =
  "3.2";

std::istream&
operator>>(std::istream& is, ActionRequestKey& key)
{
  is >> key.sdate_;
  read_eol(is);
  is >> key.colo_id_;
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ActionRequestKey& key)
  throw(eh::Exception)
{
  os << key.sdate_ << '\n' << key.colo_id_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ActionRequestInnerKey& key)
  throw(eh::Exception)
{
  key.holder_ = new ActionRequestInnerKey::DataHolder();
  TokenizerInputArchive<> ia(is);
  ia >> *key.holder_;
  key.holder_->calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ActionRequestInnerKey& key)
  throw(eh::Exception)
{
  TabOutputArchive oa(os);
  oa << *key.holder_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, ActionRequestInnerData& data)
{
  is >> data.action_request_count_;
  is >> data.cur_value_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const ActionRequestInnerData& data)
{
  os << data.action_request_count_ << '\t';
  os << data.cur_value_;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

