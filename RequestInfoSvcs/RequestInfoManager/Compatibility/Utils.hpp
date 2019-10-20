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

#ifndef REQUESTINFOMANAGER_COMPATIBILITY_UTILS_HPP
#define REQUESTINFOMANAGER_COMPATIBILITY_UTILS_HPP

#include <Generics/Time.hpp>

namespace AdServer
{
namespace RequestInfoSvcs
{
  Generics::Time
  date_month_trunc(const Generics::Time& time);
}
}

namespace AdServer
{
namespace RequestInfoSvcs
{
  inline
  Generics::Time
  date_month_trunc(const Generics::Time& time)
  {
    Generics::ExtendedTime date_day_ex(time.get_gm_time());
    
    return Generics::ExtendedTime(
      date_day_ex.tm_year + 1900,
      date_day_ex.tm_mon + 1,
      1,
      0,
      0,
      0,
      0);
  }
}
}

#endif /*REQUESTINFOMANAGER_COMPATIBILITY_UTILS_HPP*/
