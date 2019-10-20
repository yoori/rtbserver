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


#include "Request.hpp"
#include <LogCommons/LogCommons.ipp>

namespace AdServer {
namespace LogProcessing {

template <> const char* RequestTraits::B::base_name_ = "Request";
template <> const char* RequestTraits::B::signature_ = "Request";
template <> const char* RequestTraits::B::current_version_ = "3.7.1";

namespace {

const char CC_SEP1 = ',';
const char CC_SEP2 = ':';

const char UP_SEP1 = ',';
const char UP_SEP2 = '=';

} // namespace

const RequestData_V_3_5_4::DeliveryThresholdT
  RequestData_V_3_5_4::DataHolder::max_delivery_threshold_value_("1.00000");

const RequestData_V_3_6::DeliveryThresholdT
  RequestData_V_3_6::DataHolder::max_delivery_threshold_value_("1.00000");

const RequestData::DeliveryThresholdT
  RequestData::DataHolder::max_delivery_threshold_value_("1.00000");

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, RequestData& data)
  throw(eh::Exception)
{
  data.holder_ = new RequestData::DataHolder;
  TokenizerInputArchive</*Aux_::OwnInvariants*/> ia(is);
  ia >> *data.holder_;
  return is;
}

std::ostream&
operator<<(std::ostream& os, const RequestData& data)
  throw(eh::Exception)
{
  TabOutputArchive oa(os);
  oa << *data.holder_;
  return os;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, RequestData_V_3_5_4& data)
  throw(eh::Exception)
{
  data.holder_ = new RequestData_V_3_5_4::DataHolder;
  TokenizerInputArchive<> ia(is);
  ia >> *data.holder_;
  return is;
}

FixedBufStream<TabCategory>&
operator>>(FixedBufStream<TabCategory>& is, RequestData_V_3_6& data)
  throw(eh::Exception)
{
  data.holder_ = new RequestData_V_3_6::DataHolder;
  TokenizerInputArchive<> ia(is);
  ia >> *data.holder_;
  return is;
}
} // namespace LogProcessing
} // namespace AdServer
