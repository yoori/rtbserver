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


#ifndef _AUTOTEST__PROFILESEXPIRATIONTEST_
#define _AUTOTEST__PROFILESEXPIRATIONTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
 
class ProfilesExpirationTest: public BaseUnit
{
public:
 
  ProfilesExpirationTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var), remote_case_(false)
  {};
 
  virtual ~ProfilesExpirationTest() throw()
  {};
 
private:

  bool remote_case_;

  void
  check_profiles_exist(
    const std::string& uid,
    bool exists,
    bool temp_user);

  void
  expired_visits_removal_();

  void
  user_profiles_removal_(
    AutoTest::AdClient& user1,
    unsigned long time1,
    AutoTest::AdClient& user2,
    unsigned long time2,
    bool temp_user = false);
 
  virtual bool run_test();
 
};

#endif //_AUTOTEST__PROFILESEXPIRATIONTEST_
