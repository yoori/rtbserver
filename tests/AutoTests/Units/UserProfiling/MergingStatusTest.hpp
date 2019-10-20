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

#ifndef _UNITTEST__MERGINGSTATUSTEST_
#define _UNITTEST__MERGINGSTATUSTEST_

#include <tests/AutoTests/Commons/Common.hpp>

typedef AutoTest::NSLookupRequest  NSLookupRequest;
typedef AutoTest::AdClient AdClient;
typedef AutoTest::TemporaryAdClient TemporaryAdClient;
 
class MergingStatusTest: public BaseUnit
{
public:
 
  MergingStatusTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var),
    client(AdClient::create_user(this))
  {};
 
  virtual ~MergingStatusTest() throw()
  {};
 
private:
  virtual bool run_test();
  AdClient client;
  std::string uid;

  void set_up();
  // test cases
  void double_merging();
  void unknown_tuid();
  // utils
  std::string create_temporary_profile();
  
};

#endif
