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

#ifndef _AUTOTEST__STATSHOURLYACTIONSTEST_
#define _AUTOTEST__STATSHOURLYACTIONSTEST_
  
#include <tests/AutoTests/Commons/Common.hpp>
  
namespace ORM = ::AutoTest::ORM;

class StatsHourlyActionsTest : public BaseDBUnit
{
  typedef ORM::HourlyStats HourlyStats;
  
public:
  StatsHourlyActionsTest(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseDBUnit(
      stat_var, task_name, params_var),
    target_request_time_(
      AutoTest::Time().get_gm_time().
        format("%d-%m-%Y:%H-00-00"))
  { }

  virtual ~StatsHourlyActionsTest() throw()
  { }

protected:

  virtual void set_up();
  virtual bool run();
  virtual void tear_down();

private:

  void case_base_functionality();//1
  void case_action_from_different_campaign(); //2.1
  void case_triple_action();//2.2
  void case_action_before_click();//3
  void case_action_before_impression_confirmation();//4
  void case_one_action_for_multiple_creatives_in_campaign();//5
  void case_action_for_display_creative_group_with_cpc_rate();//6
  void case_actions_for_text_ad_group();//7

  AutoTest::Time target_request_time_;

};

#endif // _AUTOTEST__STATSHOURLYACTIONSTEST_
