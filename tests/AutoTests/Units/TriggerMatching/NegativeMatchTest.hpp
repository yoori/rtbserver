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


#ifndef _UNITTEST__NEGATIVEMATCHTEST_
#define _UNITTEST__NEGATIVEMATCHTEST_

#include <tests/AutoTests/Commons/Common.hpp>

/**
 * @class NegativeMatchTest
 * @brief test negative match
 */

class NegativeMatchTest:  public BaseDBUnit
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::ORM::ChannelTriggerStats Stat;
  typedef AutoTest::ORM::StatsList<Stat> Stats;
  typedef Stat::Diffs Diff;
  typedef std::list<Diff> Diffs;

public:

  struct TestCase
  {
    const char* referer;
    const char* referer_kw;
    const char* matched;
    const char* unmatched;
    const char* ccid;
  };
  
public:
 
  NegativeMatchTest(UnitStat& stat_var, 
                    const char* task_name, 
                    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var),
      conn_(open_pq())
  { }
 
  virtual ~NegativeMatchTest() throw()
  { }
 
private:

  AutoTest::DBC::Conn conn_;

  Stats stats_;
  Diffs diffs_;

  void set_up();
  void pre_condition();
  bool run();
  void post_condition();
  void tear_down();

  template<size_t Count>
  void
  test_group(
    const std::string& description,
    const TestCase(&tests)[Count]);

  void
  test_case(
    const std::string& description,
    AdClient& client,
    const TestCase& test);

};

#endif // _UNITTEST__NEGATIVEMATCHTEST_

