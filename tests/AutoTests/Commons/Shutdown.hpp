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


#ifndef _AUTOTESTS_COMMONS_SHUTDOWN_HPP
#define _AUTOTESTS_COMMONS_SHUTDOWN_HPP

#include <Generics/Singleton.hpp>
#include <Sync/Condition.hpp>
#include "Request/BaseRequest.hpp"

namespace AutoTest
{
  DECLARE_EXCEPTION(ShutdownException, eh::DescriptiveException);
  
  class Shutdown_
  {
  public:

    Shutdown_();
    
    void set()
      throw (eh::Exception);
    
    void wait(
      const Time& timeout)
      throw (ShutdownException, eh::Exception);

    bool get();


  private:
    Sync::Condition cond_;
    volatile sig_atomic_t value_;
  };

  typedef Generics::Singleton<Shutdown_> Shutdown;
}


#endif  // _AUTOTESTS_COMMONS_SHUTDOWN_HPP
