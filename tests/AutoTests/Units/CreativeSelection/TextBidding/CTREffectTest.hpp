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

#ifndef _UNITTEST__CTRCALCULATIONLOGICTEST_
#define _UNITTEST__CTRCALCULATIONLOGICTEST_

#include <tests/AutoTests/Commons/Common.hpp>


class CTREffectTest: public BaseUnit
{
public:
 
  CTREffectTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  {};
 
private:

  struct TestCaseRequest
  {
    std::string tag;
    AutoTest::NSLookupRequest::Member request_param_name;
    std::string request_param_value;
    std::string expected_ccids;
    std::string click_revenue;
  };

  bool run_test();

  // helper funcs
  template<size_t Count>
  void process_requests(const TestCaseRequest(&requests)[Count],
                        const std::string& prefix = std::string());

  // Test cases
  void regular_adjustment_();
  void invalid_ctr_();
  void ccg_concurrence_();
  void negative_ctr_();
  void time_of_week_();
};

#endif // _UNITTEST__CTRCALCULATIONLOGICTEST_
