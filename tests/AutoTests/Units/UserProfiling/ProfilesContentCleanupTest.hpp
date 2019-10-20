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


#ifndef _AUTOTEST__PROFILESCONTENTCLEANUPTEST_
#define _AUTOTEST__PROFILESCONTENTCLEANUPTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
class ProfilesContentCleanupTest: public BaseUnit
{
public:
 
  ProfilesContentCleanupTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var),
    base_time_(
      (AutoTest::Time().get_gm_time().format("%d-%m-%Y") +
        ":" + "00-01-00").c_str())
  {};
 
  virtual ~ProfilesContentCleanupTest() throw()
  {};
 
private:

  AutoTest::Time base_time_;
  std::string channel_,
              cc_id_,
              ccg_id_;

  void cleanup_by_visits_count();
  void cleanup_by_visits_date();

  virtual bool run_test();
 
};

#endif //_AUTOTEST__PROFILESCONTENTCLEANUPTEST_
