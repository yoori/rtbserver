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


#ifndef _AUTOTEST__KEYWORDSNORMALIZATIONTEST_
#define _AUTOTEST__KEYWORDSNORMALIZATIONTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
 
class KeywordsNormalizationTest: public BaseDBUnit
{

  typedef AutoTest::ORM::ChannelTriggerStats TriggerStat;
  typedef AutoTest::ORM::StatsList<TriggerStat> TriggerStats;
  typedef TriggerStat::Diffs TriggerDiff;
  typedef std::list<TriggerDiff> TriggerDiffs;

  typedef AutoTest::ORM::ChannelInventoryStats ChannelStat;
  typedef AutoTest::ORM::StatsList<ChannelStat> ChannelStats;
  typedef ChannelStat::Diffs ChannelDiff;
  typedef std::list<ChannelDiff> ChannelDiffs;

public:
 
  KeywordsNormalizationTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var),
    conn_(open_pq())
  {};
 
  virtual ~KeywordsNormalizationTest() throw()
  {};
 
private:

  AutoTest::DBC::Conn conn_;

  TriggerStats trigger_stats_;
  TriggerStats unexpected_trigger_stats_;
  TriggerDiffs trigger_diffs_;

  ChannelStats channel_stats_;
  ChannelDiffs channel_diffs_;

  void set_up();
  void pre_condition();
  bool run();
  void post_condition();
  void tear_down();
 
};

#endif //_AUTOTEST__KEYWORDSNORMALIZATIONTEST_
