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


#ifndef __AUTOTESTS_COMMONS_ADMINS_SEARCHENGINECHECKER_HPP
#define __AUTOTESTS_COMMONS_ADMINS_SEARCHENGINECHECKER_HPP

#include <tests/AutoTests/Commons/Admins/SearchEngineAdmin.hpp>
#include <tests/AutoTests/Commons/Checkers/AdminsChecker.hpp>
#include <tests/AutoTests/Commons/Checkers/Checker.hpp>

namespace AutoTest
{
  template<typename ExpectedType = SearchEngineAdmin::Expected>  
  class SearchEngineChecker_:  
    public AutoTest::Checker  
  {  
    public:    

      typedef ExpectedType Expected;      

      SearchEngineChecker_(      
        BaseUnit* test,      
        unsigned long id,      
        const Expected& expected,      
        AdminExistCheck exists = AEC_EXISTS) :      
        test_(test),      
        id_(id),      
        expected_(expected),      
        exists_(exists)      
        {}      

      virtual ~SearchEngineChecker_() throw() {}      

      bool check(bool throw_error = true) throw (CheckFailed, eh::Exception);      

    private:    

      BaseUnit* test_;      
      unsigned long id_;      
      Expected expected_;      
      AdminExistCheck exists_;      

  };  

  typedef SearchEngineChecker_<SearchEngineAdmin::Expected> SearchEngineChecker;  
  typedef SearchEngineChecker_<std::string> SearchEngineCheckerSimple;  

}

#include "SearchEngineChecker.tpp"

#endif  // __AUTOTESTS_COMMONS_ADMINS_SEARCHENGINECHECKER_HPP

