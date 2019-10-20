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


#pragma once

#include "CookieManager.hpp"
#include "Cookies.hpp"

namespace FrontendCommons
{
  template<typename HttpRequestType, typename HttpResponseType>
  inline void
  add_UID_cookie(
    HttpResponseType& response,
    const HttpRequestType& request,
    const FrontendCommons::CookieManager<
      HttpRequestType, HttpResponseType>& cookie_manager,
    const std::string& signed_client_id)
    throw (eh::Exception)
  {
    if(!signed_client_id.empty())
    {
      /* renew expiration time for uid cookie */
      cookie_manager.set(
        response,
        request,
        FrontendCommons::Cookies::CLIENT_ID,
        signed_client_id);
    }
  }

}
