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

#ifndef _AUTOTEST__OPTOUTADVERTISING_
#define _AUTOTEST__OPTOUTADVERTISING_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class OptoutAdvertising : public BaseUnit
{
  
public:

  struct TestCase {
    const char* colo;
    const char* optin_ccid;
    const char* undef_ccid;
    const char* optout_ccid;
    unsigned short flags;
  };

  typedef AutoTest::NSLookupRequest NSLookupRequest;
  
public:
  OptoutAdvertising(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var)
  {};

  virtual ~OptoutAdvertising() throw()
  {};

private:

  virtual bool run_test();

  template <size_t Count>
  void
  run_test_case(
    const TestCase(&testcases)[Count],
    const NSLookupRequest& base_request);

  void optout_click_and_impression();
  
};

#endif // _AUTOTEST__OPTOUTADVERTISING_

