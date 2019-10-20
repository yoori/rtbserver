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

#ifndef _UNITTEST__CONTEXTCHANNELSTEST_
#define _UNITTEST__CONTEXTCHANNELSTEST_

#include <tests/AutoTests/Commons/Common.hpp>
 
typedef std::list <std::string> ChannelList;

class ContextChannelsTest: public BaseUnit
{
public:
 
  ContextChannelsTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var): 
    BaseUnit(stat_var, task_name, params_var),
    test_client_(AutoTest::AdClient::create_user(this)),
    req_count_(0)
  {};
 
  virtual ~ContextChannelsTest() throw()
  {};
protected:

  AutoTest::AdClient test_client_; // AdServer client
private:

  unsigned long req_count_;                     // Client request count
  
  virtual bool run_test();

  // test cases
  void url_request_test_case();
  void page_request_test_case();
  void search_request_test_case();
  void composite_request_test_case();
};

#endif
