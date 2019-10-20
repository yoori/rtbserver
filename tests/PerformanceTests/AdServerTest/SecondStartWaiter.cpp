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


#include "SecondStartWaiter.hpp"

// SecondStartWaiter class

SecondStartWaiter::SecondStartWaiter()  :
  second_start_time_(Generics::Time::get_time_of_day())
{ }

SecondStartWaiter::~SecondStartWaiter()
{
  Guard_ guard(condition_);
  Generics::Time wake_up_time(second_start_time_ + WAIT_DURATION);
  guard.timed_wait(&wake_up_time);
}
