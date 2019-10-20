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

/* $Id$
* @file SiteUpdateTest.hpp
* Test that server dynamically update Site.
* For more info see https://confluence.ocslab.com/display/ADS/SiteUpdateTest
*/
#ifndef AUTOTESTS_UNITS_SITEUPDATETEST_HPP
#define AUTOTESTS_UNITS_SITEUPDATETEST_HPP

#include <tests/AutoTests/Commons/Common.hpp>

/**
 * @class SiteUpdateTest
 * @brief Test for site updates
 */ 
class SiteUpdateTest: public BaseDBUnit
{
public:
  SiteUpdateTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var)
  { }
 
  virtual ~SiteUpdateTest() throw()
  { }
 
private:

  AutoTest::Time base_time_;

  void set_up();
  void tear_down();
  bool run();

  void create_site_();
  void update_site_campaign_approval_();
  void update_noads_timeout_();
  void update_creative_exclusion_();
  void delete_site_();
  void update_site_freq_caps_();
};

#endif /*AUTOTESTS_UNITS_SITEUPDATETEST_HPP*/
