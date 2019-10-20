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

#ifndef _UNITTEST__DAILYRUNTEST_
#define _UNITTEST__DAILYRUNTEST_

#include <tests/AutoTests/Commons/Common.hpp>

 
class DailyRunTest: public BaseUnit
{

public:

  enum WeekDays
  {
    WD_MONDAY  = 0,
    WD_TUESDAY = 1,
    WD_WEDNESDAY = 2,
    WD_THURSDAY = 3,
    WD_FRIDAY = 4,
    WD_SATURDAY = 5,
    WD_SUNDAY = 6,
    WD_CURRENT = 7
  };
  
  struct RequestTimeResult
  {
    WeekDays wday;
    const char *time;
    bool have_cc;
  };
  
public:
 
  DailyRunTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~DailyRunTest() throw()
  {};
 
private:
 
  virtual bool run_test();

  void process_testcase(
    const char *keyword,
    const char *tz_name,
    const RequestTimeResult expect[],
    size_t expect_count);

  Generics::Time
  get_gmt(
    DailyRunTest::WeekDays wday,
    const char* local_tm,
    const char* time_zone);

};

#endif
