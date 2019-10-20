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

#ifndef AUTOTESTS_CHANNELHITTEST_H
#define AUTOTESTS_CHANNELHITTEST_H

#include <tests/AutoTests/Commons/Common.hpp>


class ChannelHitTest : public BaseDBUnit
{

  typedef AutoTest::ORM::ChannelInventoryStats Stat;
  typedef AutoTest::ORM::StatsList<Stat> Stats;
  typedef AutoTest::ORM::ChannelInventoryStats::Diffs Diff;
  typedef std::list<Diff> Diffs;

public:
  ChannelHitTest(UnitStat& stat_var, const char* task_name,
            XsdParams params_var)
  : BaseDBUnit(stat_var, task_name, params_var),
  default_colo_(fetch_int("DefaultColo")),
  base_time_()
  {}

  virtual ~ChannelHitTest() throw()
  {}

private:

  const unsigned long default_colo_;
  AutoTest::Time base_time_;

  Stats stats_;
  Diffs diffs_;

  bool run();
  void tear_down();
  void run_case(const AutoTest::Time& date,
    unsigned long colo = 0);
};

#endif // AUTOTESTS_CHANNELHITTEST_H
