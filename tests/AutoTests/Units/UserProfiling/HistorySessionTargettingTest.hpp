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

#ifndef _UNITTEST__HISTORYSESSIONTARGETTINGTEST_
#define _UNITTEST__HISTORYSESSIONTARGETTINGTEST_

#include <tests/AutoTests/Commons/Common.hpp>
 
class HistorySessionTargettingTest:
  public BaseUnit
{
public:

  struct ExpectedType
  {
    const char* triggers;
    const char* history;    
  };

  struct RequestType
  {
    long time_ofset;
    const char* referer_kw;
    ExpectedType expected;
    ExpectedType unexpected;
    bool log_request;
  };
  
public:
 
  HistorySessionTargettingTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var): 
    BaseUnit(stat_var, task_name, params_var),
    base_time(
      (AutoTest::Time().get_gm_time().format("%d-%m-%Y") +
        ":" + "12-00-00").c_str())
  {};

  virtual ~HistorySessionTargettingTest() throw()
  {};
   
private:

  AutoTest::Time base_time;

  virtual bool run_test();

  void check(
    const RequestType* requests,
    size_t requests_size,
    const AutoTest::Time& base_time);

};

#endif
