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

#ifndef _UNITTEST__CCGKEYWORDSTATSTEST_
#define _UNITTEST__CCGKEYWORDSTATSTEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = AutoTest::ORM;

class CCGKeywordStatsTest: public BaseDBUnit
{

  enum StatOption
  {
    SO_DUMP_TOTAL = 1  // Create total stat & diff
  };
  
public:
  CCGKeywordStatsTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var),
      current_total_(0)
  { }
 
  virtual ~CCGKeywordStatsTest() throw()
  { }

protected:

  virtual void set_up();
  
  virtual bool run();

  virtual void tear_down();
 
private:

  // Cases

  void
  system_currency_case_();
  
  void
  advertiser_currency_case_();
  
  void
  mixed_currency_case_();
  
  void
  advertiser_timezone_case_();

  // Test utils
  
  void 
  add_stats_(
    const std::string& description,
    const ORM::CCGKeywordStatsHourly::Key& key,
    const ORM::CCGKeywordStatsHourly::Diffs& diff,
    unsigned long flags = SO_DUMP_TOTAL);

 
private:

  ORM::StatsList<ORM::CCGKeywordStatsTotal> total_;
  ORM::StatsList<ORM::CCGKeywordStatsHourly> hourly_;
  ORM::StatsList<ORM::CCGKeywordStatsToW> week_;
  ORM::StatsList<ORM::CCGKeywordStatsDaily> daily_;
  std::list<ORM::CCGKeywordStatsHourly::Diffs> diffs_;
  std::list<ORM::CCGKeywordStatsTotal::Diffs> total_diffs_;
  ORM::CCGKeywordStatsTotal::Diffs current_total_;

};

#endif
