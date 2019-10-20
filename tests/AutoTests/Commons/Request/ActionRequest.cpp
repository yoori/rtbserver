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

namespace AutoTest
{
  const char* ActionRequest::BASE_URL  = "/services/ActionServer/";
  const char* ActionRequest::COUNTRY_CODE = "gn";

  ActionRequest::ActionRequest(bool set_defs) :
    BaseRequest(BASE_URL, BaseRequest::RT_NOT_ENCODED),
    cid(this, "cid", 0, false),
    actionid(this, "actionid", 0, false),
    country(this, "country", COUNTRY_CODE, set_defs),
    testrequest(this, "testrequest", 0, set_defs),
    debug_time(this, "debug-time", 0, false),
    referer(this, "Referer", 0, false)
  { }

  ActionRequest::ActionRequest(const ActionRequest& other) :
    BaseRequest(BASE_URL, BaseRequest::RT_NOT_ENCODED),
    cid(this, other.cid),
    actionid(this, other.actionid),
    country(this, other.country),
    testrequest(this, other.testrequest),
    debug_time(this, other.debug_time),
    referer(this, other.referer)
  { }
}



