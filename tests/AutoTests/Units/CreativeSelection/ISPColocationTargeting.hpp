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


#ifndef _AUTOTEST__ISPCOLOCATIONTARGETING_
#define _AUTOTEST__ISPCOLOCATIONTARGETING_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class ISPColocationTargeting : public BaseUnit
{
  struct TestCase
  {
    const char* colo;
    const char* referer_kw;
    const char* expected_history;
    const char* expected_ccs;
  };
  
public:
  ISPColocationTargeting(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var)
  { }

  virtual ~ISPColocationTargeting() throw()
  { }

private:

  virtual bool run_test();

  template <size_t Count>
  void
  process_case(
    const std::string& description,
    const TestCase(&testcases)[Count]);
  
};

#endif // _AUTOTEST__ISPCOLOCATIONTARGETING_
