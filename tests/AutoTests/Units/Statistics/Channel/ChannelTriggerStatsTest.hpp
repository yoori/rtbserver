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


#ifndef _UNITTEST__CHANNELTRIGGERSTATLOGGINGTEST_
#define _UNITTEST__CHANNELTRIGGERSTATLOGGINGTEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = ::AutoTest::ORM;
namespace DB = ::AutoTest::DBC;

/**
 * @class ChannelTriggerStatsTest
 * @brief test channel trigger stat logging behaviour
 * see https://confluence.ocslab.com/display/ADS/ChannelTriggerStatsTest
 */  
class ChannelTriggerStatsTest: public BaseDBUnit
{
  typedef ORM::ChannelTriggerStats Stat;
  typedef ORM::StatsList<Stat> Stats;
  typedef Stat::Diffs Diff;
  typedef std::list<Diff> Diffs;

public:
  ChannelTriggerStatsTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var),
    default_colo_(fetch_int("DefaultColo"))
  { }
 
  virtual ~ChannelTriggerStatsTest() throw()
  { }

protected:

  virtual bool run();

  virtual void set_up();
  
  virtual void tear_down();

private:

  AutoTest::Time now_;

  struct ChannelTriggerStatsRow
  {
    unsigned long colo_id;
    const char* channel_trigger_id;
    const char* trigger_type;
    int hits;
  };

  struct ChannelTriggerRequest
  {
    unsigned long colo;
    unsigned long tid;
    unsigned long ccid;
    const char* referer_kw;
    const char* referer;
    const char* search;
    const char* ft;
  };

  unsigned long default_colo_;

  void no_tid_case_();
  void with_ad_case_();
  void url_kwd_();
  void adsc_6348_();
  void adsc_7962_();

  // help funs
  template<size_t Count>
  void add_stats_(
    Stats& stats,
    Diffs& diffs,
    const ChannelTriggerStatsRow (&expected)[Count]);

  template<size_t Count>
  void process_requests_(
    const ChannelTriggerRequest (&requests)[Count]);
};

#endif
