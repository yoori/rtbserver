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


#ifndef _AUTOTEST__EXACTKEYWORDMATCHING_
#define _AUTOTEST__EXACTKEYWORDMATCHING_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class ExactKeywordMatching : public BaseUnit
{

  typedef AutoTest::AdClient AdClient;

public:
  
  struct TestCase
  {
    const char* search;
    const char* matched;
    const char* unmatched;
  };

public:
  ExactKeywordMatching(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var),
    case_idx_(0)
  { }

  virtual ~ExactKeywordMatching() throw()
  { }

private:

  unsigned long case_idx_;

  virtual bool run_test();

  template<size_t Count>
  void
  test_group_(
    AdClient& client,
    const TestCase(&tests)[Count],
    unsigned long flags = 0);

  void
  test_case_(
    AdClient& client,
    const TestCase&test,
    unsigned long flags);

};

#endif // _AUTOTEST__EXACTKEYWORDMATCHING_
