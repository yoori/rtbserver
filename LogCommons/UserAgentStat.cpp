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


#include "UserAgentStat.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* UserAgentStatTraits::B::base_name_ = "UserAgentStat";
template <> const char* UserAgentStatTraits::B::signature_ = "UserAgentStat";
template <> const char* UserAgentStatTraits::B::current_version_ = "2.5";

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, UserAgentStatInnerKey& key)
  throw(eh::Exception)
{
  Aux_::StringIoWrapper user_agent_wrapper;
  is >> user_agent_wrapper;
  key.user_agent_ =
    new AdServer::Commons::StringHolder(std::move(user_agent_wrapper));
  key.invariant();
  key.calc_hash_();
  return is;
}

std::ostream&
operator<<(std::ostream& os, const UserAgentStatInnerKey& key)
  throw(eh::Exception)
{
  key.invariant();
  os << Aux_::StringIoWrapper(key.user_agent_->str());
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, UserAgentStatInnerData& data)
  throw(eh::Exception)
{
  data.holder_ = new UserAgentStatInnerData::DataHolder;
  is >> data.requests_;
  is >> data.holder_->channels;
  is >> data.holder_->platforms;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const UserAgentStatInnerData& data)
  throw(eh::Exception)
{
  os << data.requests_ << '\t';
  os << data.holder_->channels << '\t';
  os << data.holder_->platforms;
  return os;
}

} // namespace LogProcessing
} // namespace AdServer

