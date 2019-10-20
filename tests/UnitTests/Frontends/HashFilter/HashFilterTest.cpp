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

#include <iostream>
#include <eh/Exception.hpp>

#include <Frontends/ProfilingServer/HashFilter.hpp>

namespace UnitTests
{
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
}

int
main(/*int argc, char* argv[]*/)
{
  AdServer::Profiling::HashFilter_var hash_filter = new AdServer::Profiling::HashFilter(
    100,
    1000,
    Generics::Time(10),
    Generics::Time::ONE_MINUTE);

  Generics::Time now = Generics::Time::get_time_of_day();

  // check now filtering
  std::cout << "NOW: " << now.gm_ft() << std::endl;

  bool res = hash_filter->set(10000, now, now);
  std::cout << "set #1.1(, " << now.gm_ft() << "): " << res << std::endl;

  res = hash_filter->set(10000, now, now);
  std::cout << "set #1.2(, " << now.gm_ft() << "): " << res << std::endl;

  // check already expired (both false)
  res = hash_filter->set(10000, now - Generics::Time::ONE_MINUTE, now);
  std::cout << "set #2.1(, " << now.gm_ft() << "): " << res << std::endl;

  res = hash_filter->set(10000, now - Generics::Time::ONE_MINUTE, now);
  std::cout << "set #2.2(, " << now.gm_ft() << "): " << res << std::endl;

  // check last second before expiration
  res = hash_filter->set(10000, now - Generics::Time::ONE_MINUTE + Generics::Time::ONE_SECOND * 11, now);
  std::cout << "set #3.1(, " << now.gm_ft() << "): " << res << std::endl;

  res = hash_filter->set(10000, now - Generics::Time::ONE_MINUTE + Generics::Time::ONE_SECOND * 11, now);
  std::cout << "set #3.2(, " << now.gm_ft() << "): " << res << std::endl;


  res = hash_filter->set(10000, now - Generics::Time(10), now);
  std::cout << "set #4.1(, " << now.gm_ft() << "): " << res << std::endl;

  res = hash_filter->set(10000, now - Generics::Time(10), now);
  std::cout << "set #4.2(, " << now.gm_ft() << "): " << res << std::endl;


  res = hash_filter->set(10000, now - Generics::Time(10), now + Generics::Time::ONE_SECOND * 30);
  std::cout << "set #5(, " << (now + Generics::Time::ONE_SECOND * 30).gm_ft() << "): " << res << std::endl;


  res = hash_filter->set(10000000, now - Generics::Time(10), now + Generics::Time::ONE_SECOND * 30);
  std::cout << "set #6.1(, " << (now + Generics::Time::ONE_SECOND * 30).gm_ft() << "): " << res << std::endl;

  res = hash_filter->set(10000000, now - Generics::Time(10), now + Generics::Time::ONE_SECOND * 30);
  std::cout << "set #6.2(, " << (now + Generics::Time::ONE_SECOND * 30).gm_ft() << "): " << res << std::endl;

  return 0;
}

