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

#ifndef AUTOTESTS_UNITS_CHANNELTRIGGERIMPSTATSTEST_HPP
#define AUTOTESTS_UNITS_CHANNELTRIGGERIMPSTATSTEST_HPP

#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = AutoTest::ORM;
namespace DBC = ::AutoTest::DBC;

typedef AutoTest::ORM::ChannelTriggerStats ChannelTriggerStats;
typedef ORM::StatsList<ChannelTriggerStats> ChannelTriggerStatsArray;
typedef std::map<unsigned long, unsigned long> TriggerCounterMap;
typedef ORM::StatsArray<ChannelTriggerStats, 1> ChannelTriggerSumStats;
 
class ChannelTriggerImpStatsTest: public BaseDBUnit
{
public:
  ChannelTriggerImpStatsTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var)
  { }
 
  virtual ~ChannelTriggerImpStatsTest() throw()
  { }


protected:

  virtual bool run();

  virtual void set_up();
  
  virtual void tear_down();

private:
  AutoTest::Time time_;

  void fetch_stats_(
    ChannelTriggerStatsArray& triggers_stats,
    TriggerCounterMap& imps_done,
    ChannelTriggerSumStats& sum_stats,
    const char* entity_prefix,
    unsigned long imps_count);

  void do_requests_for_one_trigger_case_(
    const char* entity_prefix,
    unsigned long imps_count,
    unsigned long clicks_count);

  void do_requests_for_one_trigger_tempo_case_(
    const char* entity_prefix,
    unsigned long imps_count,
    unsigned long clicks_count);

  void channel_types();
};

#endif /*AUTOTESTS_UNITS_CHANNELTRIGGERIMPSTATSTEST_HPP*/
