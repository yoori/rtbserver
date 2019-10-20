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

#ifndef ORACLE_TIMEOUTCONTROL_HPP
#define ORACLE_TIMEOUTCONTROL_HPP

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include "OraException.hpp"

namespace AdServer
{
namespace Commons
{
  namespace Oracle
  {
    class TimeoutControl
    {
    public:
      TimeoutControl(const Generics::Time* timeout = 0) throw();
        
      bool sleep_step() throw(Exception, TimedOut);

      Generics::Time passed_time() const throw();

      Generics::Time timeout() const throw();

    private:
      static const Generics::Time MIN_SLEEP_TIME;
      static const Generics::Time MAX_SLEEP_TIME;
        
    private:
      Generics::Time timeout_;
      Generics::Time start_time_;
    };
  }
}
}

#endif /*ORACLE_TIMEOUTCONTROL_HPP*/
