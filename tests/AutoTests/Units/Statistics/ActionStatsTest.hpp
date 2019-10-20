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


#ifndef _UNITTEST__ACTIONSTATSTEST_
#define _UNITTEST__ACTIONSTATSTEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace DB = AutoTest::DBC;
namespace ORM = AutoTest::ORM;

/**
 * @class ActionStatsTest
 * @brief Test logging into ActionStats table
 */ 
class ActionStatsTest:
  public BaseDBUnit
{

  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::ActionRequest ActionRequest;
  typedef AutoTest::ConversationRequest ConversationRequest;
  
  struct ConversationStat
  {
    const char* action;
    const char* cc;
    const char* tid;
    double cur_value;
    const char* order_id;
  };

  struct ConversationAction
  {
    const char* action;
    const char* value;
    unsigned int status;
  };


  struct CaseStat
  {
    const char* action;
    const char* cc;
    const char* tid;
    const char* colo;
    const char* country;
    int time_ofset;
    int imp_ofset;
    int click_ofset;
  };

  
public:
  ActionStatsTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var),
      base_time(
        AutoTest::Time().get_gm_time().format("%d-%m-%Y:%H-00-00"))
  { }
 
  virtual ~ActionStatsTest() throw()
  { }
 
protected:

  virtual bool run();

  virtual void set_up();
  
  virtual void tear_down();

private:
  AutoTest::Time base_time;

  // Utils
  void
  initialize_stat_(
    ORM::ActionStats& stat,
    const CaseStat& expected);

  void
  initialize_stat_(
    ORM::ActionRequests& stat,
    const CaseStat& expected);

  void
  initialize_stat_(
    ORM::ActionStats& stat,
    const ConversationStat& expected);

  void
  initialize_stat_(
    ORM::ActionRequests& stat,
    const char* action);

  template<class Stat, class Expected, size_t Count>
  void
  initialize_stats_(
    ORM::StatsList<Stat>& stats_array,
    const Expected(&case_stats) [Count]);

  template<size_t Count>
  void
  process_conversations_(
    AdClient& client,
    ConversationRequest::Member param,
    const ConversationAction(&requests) [Count]);

  // Cases
  void
  base_case_part_1_(
    AdClient& client,
    std::list<std::string>& imps,
    std::list<std::string>& clicks);

  void
  base_case_part_2_(
    AdClient& client,
    std::list<std::string>& imps);

  void
  base_case_part_3_(
    AdClient& client,
    std::list<std::string>& clicks);

  void
  base_case_part_4_(
    AdClient& client);
  
  void
  cross_action_();
  
  void
  imp_update_();

  void
  text_ads_();

  void
  deleted_action_();

  void
  expired_profile_();

  void
  conversation_value_(
    AdClient& client);

  void
  conversation_orderid_(
    AdClient& client);

  void
  referrer_test_(
    AdClient& client);

};

#endif
