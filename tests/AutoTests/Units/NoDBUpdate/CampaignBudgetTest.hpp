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

#ifndef _AUTOTEST__PAUSINGCAMPAIGNREACHINGTEST_
#define _AUTOTEST__PAUSINGCAMPAIGNREACHINGTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>

/**
 * CampaignBudgetTest can be run only once for day or
 * after data refetching
 */
class CampaignBudgetTest: public BaseDBUnit
{
public:

  CampaignBudgetTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var)
  {}

  virtual ~CampaignBudgetTest() throw()
  {}

private:

  Generics::Time base_time_;
  std::map<std::string, double> realized_budget;

  bool run();
  void post_condition();
  void tear_down();

  void fixed_daily_budget(bool initial = true);
  void dynamic_daily_budget(bool inital = true);

  void fixed_daily_budget_update();
  void dynamic_daily_budget_update();

  bool checker_call(
    const std::string& description,
    AutoTest::Checker* checker) throw(eh::Exception);
};

#endif //_AUTOTEST__PAUSINGCAMPAIGNREACHINGTEST_
