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


#ifndef __PRIMARYCHANNELSMATCHINGTEST_HPP
#define __PRIMARYCHANNELSMATCHINGTEST_HPP

#ifndef _AUTOTEST__PRIMARYCHANNELSMATCHINGTEST_
#define _AUTOTEST__PRIMARYCHANNELSMATCHINGTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
 
class PrimaryChannelsMatchingTest: public BaseUnit
{

  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest  NSLookupRequest;

public:
  struct TestCase
  {
    std::string description;
    const char* param1;
    const char* param2;
    const char* matched;
    const char* unmatched;
  };

public:
 
  PrimaryChannelsMatchingTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~PrimaryChannelsMatchingTest() throw()
  {};
 
private:
 
  virtual bool run_test();

  template<size_t Count>
  void
  test_group(
    const TestCase(&tests)[Count],
    NSLookupRequest::Member member1,
    NSLookupRequest::Member member2,
    unsigned long flags = 0);

  void
  test_case(
    AdClient& client,
    const TestCase& test,
    NSLookupRequest::Member member1,
    NSLookupRequest::Member member2,
    unsigned long flags);
 
};

#endif //_AUTOTEST__PRIMARYCHANNELSMATCHINGTEST_


#endif  // __PRIMARYCHANNELSMATCHINGTEST_HPP
