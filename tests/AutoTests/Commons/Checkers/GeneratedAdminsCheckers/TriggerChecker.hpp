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


#ifndef __AUTOTESTS_COMMONS_ADMINS_TRIGGERCHECKER_HPP
#define __AUTOTESTS_COMMONS_ADMINS_TRIGGERCHECKER_HPP

#include <tests/AutoTests/Commons/Admins/TriggerAdmin.hpp>
#include <tests/AutoTests/Commons/Checkers/AdminsChecker.hpp>
#include <tests/AutoTests/Commons/Checkers/Checker.hpp>

namespace AutoTest
{
  template<typename ExpectedType = TriggerAdmin::Expected>  
  class TriggerChecker_:  
    public AutoTest::Checker  
  {  
    public:    

      typedef ExpectedType Expected;      

      TriggerChecker_(      
        BaseUnit* test,      
        const std::string& trigger,      
        ChannelSrv service,      
        const Expected& expected,      
        AdminExistCheck exists = AEC_EXISTS) :      
        test_(test),      
        trigger_(trigger),      
        service_(service),      
        expected_(expected),      
        exists_(exists)      
        {}      

      virtual ~TriggerChecker_() throw() {}      

      bool check(bool throw_error = true) throw (CheckFailed, eh::Exception);      

    private:    

      BaseUnit* test_;      
      std::string trigger_;      
      ChannelSrv service_;      
      Expected expected_;      
      AdminExistCheck exists_;      

  };  

  typedef TriggerChecker_<TriggerAdmin::Expected> TriggerChecker;  
  typedef TriggerChecker_<std::string> TriggerCheckerSimple;  

}

#include "TriggerChecker.tpp"

#endif  // __AUTOTESTS_COMMONS_ADMINS_TRIGGERCHECKER_HPP

