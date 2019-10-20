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

#ifndef FRONTENDCOMMONS_COOKIES_HPP
#define FRONTENDCOMMONS_COOKIES_HPP

#include <String/SubString.hpp>

namespace FrontendCommons
{
  namespace Cookies
  {
    const String::SubString OPTIN("OPTED_IN");
    const String::SubString OPTOUT("OPTED_OUT");
    const String::SubString CLIENT_ID("uid");
    const String::SubString OPTOUT_TRUE_VALUE("YES");
    const String::SubString SET_UID("setuid");
  }
}

#endif /*FRONTENDCOMMONS_COOKIES_HPP*/
