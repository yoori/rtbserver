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


#ifndef _AUTOTEST__CGENDDATETEST_
#define _AUTOTEST__CGENDDATETEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
 
class CGEndDateTest: public BaseUnit
{

  enum CCGCheck
  {
    CCG_INACTIVE = 1, // CCG 'I' status check
    CCG_ACTIVE = 2    // CCG 'A' status check
  };

  struct TestRequest
  {
    std::string kwd;
    unsigned long ccid;
    unsigned long ccgid;
    unsigned short flags;
  };
  
public:
 
  CGEndDateTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  { }
 
  virtual ~CGEndDateTest() throw()
  { }
 
private:
 
  virtual bool run_test();

  // Utils
  template <size_t COUNT>
  void
  process_requests_(
    const std::string& description,
    unsigned long tag,
    const TestRequest (&requests)[COUNT]);

  // Cases

  void
  gmt_case_();

};

#endif //_AUTOTEST__CGENDDATETEST_
