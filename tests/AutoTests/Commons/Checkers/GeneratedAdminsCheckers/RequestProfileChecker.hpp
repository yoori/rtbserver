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


#ifndef __AUTOTESTS_COMMONS_ADMINS_REQUESTPROFILECHECKER_HPP
#define __AUTOTESTS_COMMONS_ADMINS_REQUESTPROFILECHECKER_HPP

#include <tests/AutoTests/Commons/Admins/RequestProfileAdmin.hpp>
#include <tests/AutoTests/Commons/Checkers/AdminsChecker.hpp>
#include <tests/AutoTests/Commons/Admins/AdminsContainer.hpp>
#include <tests/AutoTests/Commons/Checkers/Checker.hpp>

namespace AutoTest
{
  template<typename ExpectedType, CheckType ch = CT_ONE>
  class RequestProfileChecker_:  
    public AutoTest::Checker  
  {  
    public:    
            
      typedef ExpectedType Expected;

      RequestProfileChecker_(      
        BaseUnit* test,      
        const std::string& requestid,      
        RequestInfoSrv service,      
        const Expected& expected,      
        AdminExistCheck exists = AEC_EXISTS) :      
        test_(test),      
        requestid_(requestid),      
        service_(service),      
        expected_(expected),      
        exists_(exists)      
        {}      
            
      virtual ~RequestProfileChecker_() throw() {}      
            
      bool check(bool throw_error = true) throw (CheckFailed, eh::Exception);      
            
    private:    
            
      BaseUnit* test_;      
      std::string requestid_;      
      RequestInfoSrv service_;      
      Expected expected_;      
      AdminExistCheck exists_;      
            
  };  

  typedef RequestProfileChecker_<RequestProfileAdmin::Expected> RequestProfileChecker;
  typedef RequestProfileChecker_<std::string, CT_ONE_NOT_EXPECTED> RequestProfileEmptyChecker;

}

#include "RequestProfileChecker.tpp"

#endif  // __AUTOTESTS_COMMONS_ADMINS_REQUESTPROFILECHECKER_HPP

