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


#include "OptOutRequest.hpp"

namespace AutoTest
{
  const char* OptOutRequest::BASE_URL = "/services/OO";

  OptOutRequest::OptOutRequest(bool set_defs) :
    BaseRequest(BASE_URL, BaseRequest::RT_ENCODED),
    op(this, "op"),
    debug_time(this, "debug-time"),
    opted_in_url(this, "opted_in_url"),
    opted_out_url(this, "opted_out_url"),
    opt_undef_url(this, "opt_undef_url"),
    already_url(this, "already_url"),
    success_url(this, "success_url"),
    testrequest(this, "testrequest"),
    colo(
      this, "colo",
      AutoTest::GlobalSettings::instance().optout_colo(),
      set_defs),
    ct(this, "ct"),
    ce(this, "ce"),
    fail_url(this, "fail_url"),
    user_agent(this, "User-Agent", DEFAULT_USER_AGENT, set_defs)
  { }

  OptOutRequest::OptOutRequest(const OptOutRequest& other) :
    BaseRequest(BASE_URL, BaseRequest::RT_ENCODED),      
    op(this, other.op),
    debug_time(this, other.debug_time),
    opted_in_url(this, other.opted_in_url),
    opted_out_url(this, other.opted_out_url),
    opt_undef_url(this, other.opt_undef_url),
    already_url(this, other.already_url),
    success_url(this, other.success_url),
    testrequest(this, other.testrequest),
    colo(this, other.colo),
    ct(this, other.ct),
    ce(this, other.ce),
    fail_url(this, other.fail_url),
    user_agent(this, other.user_agent)
  { }
}
