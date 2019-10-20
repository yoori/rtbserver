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


#ifndef _AUTOTEST__RANDOMAUCTIONTEST_
#define _AUTOTEST__RANDOMAUCTIONTEST_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class RandomAuctionTest : public BaseDBUnit
{

  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::OrChecker OrChecker;

  struct ExpectedSequence
  {
    const char* ccs;
    double probability;
  };

  struct ExpectedEntry
  {
    const char* cc;
    double probability;
  };

public:

  struct RTBCase
  {
    const char* aid;
    const char* ccg;
    const char* size;
    const char* url;
    double bidfloor;
    int random;
    const char* expected;
    size_t count;
    unsigned short flags;
  };

  struct TanxCase
  {
    const char* aid;
    const char* ccg;
    const char* size;
    const char* url;
    double min_cpm_price;
    const char* expected;
    size_t count;
    unsigned short flags;
  };

public:
  RandomAuctionTest(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseDBUnit(stat_var, task_name, params_var)
  { }

  virtual ~RandomAuctionTest() throw()
  { }


protected:
  
  virtual
  void set_up();
  
  virtual
  bool run();
  
  virtual
  void tear_down();

private:
  // utils
  template <size_t COUNT>
  OrChecker
  prepare_checker(
    AdClient& client,
    OrChecker::ICounter* counter,
    const ExpectedSequence(&test_case)[COUNT]);

  template <size_t COUNT>
  OrChecker
  prepare_checker(
    AdClient& client,
    OrChecker::ICounter* counter,
    const ExpectedEntry(&test_case)[COUNT]);

  template <typename Expected, size_t COUNT>
  void
  test_case(
    const Expected(&expected)[COUNT],
    size_t sample_size,
    const NSLookupRequest& request);

  template <typename Traits, typename CaseType, size_t COUNT>
  void
  rtb_test_case(
    AdClient& client,
    typename Traits::Request& base_request,
    const CaseType(&rtb_cases)[COUNT]);

  // Cases
  void random_text_1();
  void random_text_2();
  void creative_size_1();
  void proportional_1();
  void proportional_2();
  void open_rtb_random(
    AdClient& client);
  void open_rtb_secondary(
    AdClient& client);
  void tanx();
};

#endif // _AUTOTEST__RANDOMAUCTIONTEST_

