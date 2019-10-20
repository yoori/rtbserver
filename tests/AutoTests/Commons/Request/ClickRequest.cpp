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


#include "ClickRequest.hpp"

namespace AutoTest
{
  const char* ClickRequest::BASE_URL = "/services/AdClickServer/";

  ClickRequest::ClickRequest() :
    BaseRequest(BASE_URL, BaseRequest::RT_NOT_ENCODED),
    ccid(this, "ccid"),
    uid(this, "uid"),
    requestid(this, "requestid"),
    ccgkeyword(this, "ccgkeyword"),
    debug_time(this, "debug-time")
  { }

  ClickRequest::ClickRequest(const ClickRequest& other) :
    BaseRequest(BASE_URL, BaseRequest::RT_NOT_ENCODED),
    ccid(this, other.ccid),
    uid(this, other.uid),
    requestid(this, other.requestid),
    ccgkeyword(this, other.ccgkeyword),
    debug_time(this, other.debug_time)
  { }

  ClickRequest::ClickRequest(const char* url)
    throw (HTTP::URLAddress::InvalidURL, eh::Exception) :
    BaseRequest(BASE_URL, BaseRequest::RT_NOT_ENCODED),
    ccid(this, "ccid"),
    uid(this, "uid"),
    requestid(this, "requestid"),
    ccgkeyword(this, "ccgkeyword"),
    debug_time(this, "debug-time")
  {
    BaseRequest::decode_(url);
  }
} // namespace AutoTest
