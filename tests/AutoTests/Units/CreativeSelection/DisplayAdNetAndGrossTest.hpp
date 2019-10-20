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

#ifndef _AUTOTEST__DISPLAYADNETANDGROSSTEST_
#define _AUTOTEST__DISPLAYADNETANDGROSSTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
namespace ORM = AutoTest::ORM;

/**
 * @class DisplayAdNetAndGrossTest
 * @brief test for creative selection in case Gross/Net accounts
 */
class DisplayAdNetAndGrossTest: public BaseDBUnit
{
public:
 
  DisplayAdNetAndGrossTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~DisplayAdNetAndGrossTest() throw()
  {};
 
private:

  typedef ORM::StatsArray<ORM::HourlyStats, 5> Stats;

  virtual void pre_condition();
  virtual bool run();
  virtual void post_condition();
  virtual void tear_down();

  void case_net_campaign_win();
  void case_gross_campaign_win();
  void case_net_campaign_win_with_commission();
  void case_publisher_commission();

  Stats stats_;
  Generics::Time target_request_time_;
};

#endif //_AUTOTEST__DISPLAYADNETANDGROSSTEST_
