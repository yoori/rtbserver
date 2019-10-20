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


#include "ConversationRequest.hpp"

namespace AutoTest
{
  const char* ConversationRequest::BASE_URL  = "/conv";

  ConversationRequest::ConversationRequest(bool set_defs) :
    BaseRequest(BASE_URL, BaseRequest::RT_ENCODED),
    convid(this, "convid", 0, false),
    value(this, "value", 0, false),
    orderid(this, "orderid", 0, false),
    test(this, "test", 0, set_defs),
    debug_time(this, "debug-time", 0, false),
    referer(this, "Referer", 0, false)
  { }

  ConversationRequest::ConversationRequest(
    const ConversationRequest& other) :
    BaseRequest(BASE_URL, BaseRequest::RT_ENCODED),
    convid(this, other.convid),
    value(this, other.value),
    orderid(this, other.orderid),
    test(this, other.test),
    debug_time(this, other.debug_time),
    referer(this, other.referer)
  { }
}
