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

#ifndef _AUTOTEST__BUDGETOVERRUNSTEST_
#define _AUTOTEST__BUDGETOVERRUNSTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
  
class BudgetOverrunsTest: public BaseDBUnit
{
public:

  enum TestCaseFlag
  {
    TCF_NOT_SPENT = 0x1,
    TCF_CPC_RATE = 0x2
  };

  typedef void (BudgetOverrunsTest::* DynamicCaseFunc)(const std::string&);

  struct TestCase
  {
    std::string prefix;
    unsigned int flags;
    unsigned int requests_count;
    DynamicCaseFunc dynamic_part;
  };

  BudgetOverrunsTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var)
  { }
 
  virtual ~BudgetOverrunsTest() throw()
  { }
 
private:

  static const TestCase TEST_CASES[];

  void process_case(size_t i);
  void campaign_zero_budget();
 
  bool run();
  void post_condition();
  void tear_down();

  // Dynamic part
  void decrease_budget_of_ccg(const std::string&);
  void decrease_budget_of_campaign(const std::string&);
  void spent_the_rest_budget(const std::string&);
  void increase_budget_of_ccg(const std::string&);
  void increase_budget_of_campaign(const std::string&);
  void set_unlim_budget_for_campaign(const std::string&);
  void set_unlim_budget_for_ccg(const std::string&);
};

#endif //_AUTOTEST__BUDGETOVERRUNSTEST_
