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


#ifndef _AUTOTEST__FREQCAPMERGINGTEST_
#define _AUTOTEST__FREQCAPMERGINGTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>

/**
 * @class FreqCapMergingTest
 * @brief Test that persistent client's frequency caps policy 
 * isn't remove after merging with temporary client
 */
class FreqCapMergingTest: public BaseUnit
{
public:

  enum RequestEnum
  {
    RE_TEMPORARY,
    RE_PERSISTENT,
    RE_MERGE
  };
      
  struct TestRequest
  {
    unsigned long time_ofset;
    RequestEnum request_type;
    const char* referer_kw;
    const char* tid;
    const char* expected_history;
    const char* expected_ccid;
  };
  
public:
  FreqCapMergingTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseUnit(stat_var, task_name, params_var)
  {};

  virtual ~FreqCapMergingTest() throw()
  {};

private:
  virtual bool run_test();

  template <size_t COUNT>
  void check(
    const std::string& description,
    const TestRequest (&requests)[COUNT]);
};
 
#endif //_AUTOTEST__FREQCAPMERGINGTEST_
