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

#ifndef _UNITTEST__USERPROFILEMERGINGTEST_
#define _UNITTEST__USERPROFILEMERGINGTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>  
 
class UserProfileMergingTest: 
  public BaseUnit
{

  typedef AutoTest::NSLookupRequest  NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::TemporaryAdClient TemporaryAdClient;
  
public:
  enum RequestEnum
  {
    RE_TEMPORARY,
    RE_PERSISTENT,
    RE_MERGING
  };

  struct ExpectedType
  {
    const char* triggers;
    const char* history;    
  };

  struct RequestType
  {
    long time_ofset;
    RequestEnum req_type;
    const char* referer_kw;
    ExpectedType expected;
    ExpectedType unexpected;
  };
  
public:
  UserProfileMergingTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~UserProfileMergingTest() throw()
  {};

  void check(
    const std::string& description,
    const RequestType* requests,
    size_t requests_size,
    const AutoTest::Time& base_time = Generics::Time::ZERO);

  void log_admin_output(
    AdClient& pclient,
    TemporaryAdClient& tclient,
    RequestEnum request_type);
 
private:
  virtual bool run_test();

private:
  AutoTest::Time base_time;
};

#endif
