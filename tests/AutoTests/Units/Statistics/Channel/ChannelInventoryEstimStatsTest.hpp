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


#ifndef _AUTOTEST__CHANNELINVENTORYESTIMSTATSTEST_
#define _AUTOTEST__CHANNELINVENTORYESTIMSTATSTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
#include "UserSet.hpp"

namespace ORM = AutoTest::ORM;
namespace DB = AutoTest::DBC;
/**
 * @class ChannelInventoryEstimStatsTest
 * @brief test channel inventory estimation statistics.
 * For more details see
 * https://confluence.ocslab.com/display/TDOCDRAFT/REQ-410+Close+to+real-time+channel+inventory+prediction
 */
class ChannelInventoryEstimStatsTest: public BaseUnit
{


  static const unsigned int USERS_COUNT = 25;
  
protected:


public:
  struct UserRequestType
  {
    unsigned long user_index;
    const char* referer;
    const char* search_kw;
    const char* referer_kw;
    const char* exp_trigger_channels;
  };

  
public:
 
  ChannelInventoryEstimStatsTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var),
    conn(open_pq())
  {};
 
  virtual ~ChannelInventoryEstimStatsTest() throw()
  {};
 
private:
 
  virtual bool run_test();

  // Helper functions
  void send_request(const UserRequestType& request,
                    unsigned int time_ofset = 0);

  // Test cases

  /**
   * @brief 1st request group (initial).
   *
   * Test ChannelInventoryEstimStats after
   * users appearance.
   */
  void test_1st_request();

  /**
   * @brief 2nd request group (70 seconds later).
   *
   * Users make some requests to make moving.
   * Test that ChannelInventoryEstimStats correctly changed.
   */
  void test_2nd_request_70s_later();

  /**
   * @brief 3d request group (130 seconds later).
   *
   * Users make some requests to make moving.
   * Test that ChannelInventoryEstimStats correctly changed.
   */
  void test_3d_request_130s_later();

  /**
   * @brief 4th request group (tomorrow).
   *
   * Users make tomorrow requests.
   * Test that ChannelInventoryEstimStats have rows for new date and
   * olddate rows isn't changed.
   */
  void test_4th_request_tomorrow();

  /**
   * @brief 5th request group (after tomorrow).
   *
   * Users make after tomorrow requests.
   * Test that ChannelInventoryEstimStats have rows for new date.
   */
  void test_5th_request_after_tomorrow();

  /**
   * @brief Users moving for request with differents colocations
   * For more details see ADSC-7276
   */
  void test_different_colo_();

  Generics::Time base_time;
  
  DB::Conn conn;

  AutoTest::Statistics::UserSet users;

  ORM::StatsArray<ORM::ChannelInventoryEstimStats, 10> dsc_stats_;

};
 
#endif //_AUTOTEST__CHANNELINVENTORYESTIMSTATSTEST_
