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

#ifndef AUTOTESTS_UNITS_GRANULARUPDATE_CAMPAIGNUPDATETEST
#define AUTOTESTS_UNITS_GRANULARUPDATE_CAMPAIGNUPDATETEST

#include <tests/AutoTests/Commons/Common.hpp>
 
namespace ORM = ::AutoTest::ORM;

/**
 * @class CampaignUpdateTest
 * @brief Test for campaign & creative updates
 */
class CampaignUpdateTest:
  public BaseDBUnit
{

public:
  CampaignUpdateTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~CampaignUpdateTest() throw()
  {};

private:
  void set_up();
  void tear_down();
  bool run();

  void add_campaign_case_();
  void currency_change_case_();
  void update_channel_case_();
  void change_status_case_();
  void change_date_interval_case_();
  void change_max_pub_share_case_();
};

#endif /*AUTOTESTS_UNITS_GRANULARUPDATE_CAMPAIGNUPDATETEST*/
