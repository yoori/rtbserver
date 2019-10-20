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

#ifndef _UNITTEST__ADDCHANNELSTEST_
#define _UNITTEST__ADDCHANNELSTEST_


#include <tests/AutoTests/Commons/Common.hpp>

namespace DB = ::AutoTest::DBC;
namespace ORM = ::AutoTest::ORM;

/**
 * @class ChannelsGrannularUpdateTest
 * @brief tests dynamic channels loading
 */ 
class ChannelsGrannularUpdateTest
  :public BaseDBUnit
{

public:

  ChannelsGrannularUpdateTest(
      UnitStat& stat_var,
      const char* task_name,
      XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var),
    chp(pq_conn_),
    che(pq_conn_),
    chs(pq_conn_)
  { }

  virtual ~ChannelsGrannularUpdateTest() throw()
  { }

private:
  
  /// Page channel
  ORM::PageChannel chp;

  /// Expression channel
  ORM::ExpressionChannel che;

  /// Search channel
  ORM::SearchChannel chs;

  // Triggers
  std::list<ORM::PQ::Triggers> triggers;


  // ChannelTrigger
  std::list<ORM::PQ::Channeltrigger> channeltriggers;
  
  void
  create_trigger(
    ORM::BehavioralChannel* channel,
    const char* kwd_name);

  void page_channel();
  void search_channel();
  void expression_channel();
  void delete_channel_expression();
  void channel_rate_change();
  
  void set_up();
  bool run();
  void tear_down();
  
  
};
 
#endif
