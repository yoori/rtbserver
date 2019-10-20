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


#include "Shutdown.hpp"

namespace AutoTest
{

  Shutdown_::Shutdown_()
    : value_(false)
  { }

  void
  Shutdown_::set()
    throw (eh::Exception)
  {
    Sync::ConditionalGuard condition(cond_);
    value_ = true;
    cond_.broadcast();   
  }
  
  void
  Shutdown_::wait(
    const Time& timeout)
    throw (ShutdownException, eh::Exception)
  {
    Sync::ConditionalGuard condition(cond_);
    if ( value_ || condition.timed_wait(&timeout, true) )
    {
      throw ShutdownException("Shutting down");
    }
  }

  bool
  Shutdown_::get()
  {
    return value_;
  }

 
}


