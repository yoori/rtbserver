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


#ifndef _AUTOTEST__MINUIDAGE_
#define _AUTOTEST__MINUIDAGE_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class MinUidAge : public BaseUnit
{

  typedef AutoTest::AdClient AdClient;
  
  struct TestCase
  {
    int ofset;
    const char* referer_kw;
    const char* expected_cc;
  };
  
public:
  MinUidAge(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var),
    base_time_(AutoTest::Time().get_gm_time().get_date())
  { }

  virtual ~MinUidAge() throw()
  { }

private:

  AutoTest::Time base_time_;

  // Utils
  template <size_t Count>
  void
  process_case(
    AdClient& client,
    const TestCase(&testcases)[Count],
    unsigned long colo = 0);

  // Test cases
  void
  uid_age_();

  void
  boundary_values_();

  void
  non_optin_();

  void
  temporary_();
    
  virtual bool run_test();

  
};

#endif // _AUTOTEST__MINUIDAGE_

