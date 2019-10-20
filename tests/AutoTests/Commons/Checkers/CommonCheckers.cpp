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


#include "CommonCheckers.hpp"

namespace AutoTest
{

  void predicate_checker(bool predicate) throw(CheckFailed)
  {
    if(!predicate)
    {
      throw CheckFailed("Fail");
    }
  }
    
  // TimeLessChecker
  TimeLessChecker::TimeLessChecker(const Generics::Time& now_less_then)
    throw()
    : now_less_then_(now_less_then)
  {}

  TimeLessChecker::~TimeLessChecker() throw()
  {}

  bool
  TimeLessChecker::check(bool throw_error)
    throw (TimeLessCheckFailed)
  {
    const Generics::Time now = Generics::Time::get_time_of_day();
    if(now < now_less_then_)
    {
      return true;
    }

    if(throw_error)
    {
      Stream::Error err;
      err << "Deadline time has expired: " << now << " (now) > "
          << now_less_then_ << " (deadline)";
      throw TimeLessCheckFailed(err);
    }

    return false;
  }
}
