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


#ifndef __SECONDSTARTWAITER_HPP
#define __SECONDSTARTWAITER_HPP

#include <Sync/Condition.hpp>
#include <Generics/Time.hpp>

class SecondStartWaiter
{
  static const unsigned short WAIT_DURATION = 1; // 1 second

protected:
  typedef Sync::ConditionalGuard Guard_;
  typedef Sync::Condition Condition_;

  mutable Condition_ condition_;

  
public:
  SecondStartWaiter();

  ~SecondStartWaiter();
  
private:
  Generics::Time second_start_time_;
};

#endif  // __SECONDSTARTWAITER_HPP
