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

#ifndef _UNITTEST__OPTOUTTEST_
#define _UNITTEST__OPTOUTTEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace
{
  typedef AutoTest::AdClient AdClient;
}

class OptOutTest: public BaseUnit
{
  
public:

  OptOutTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  { }
 
  virtual ~OptOutTest() throw()
  { }
 
private:

  virtual bool run_test();

  void base_scenario ();

  //============================================================
  //Scenarios & utils adding by changes in ADSC-53 at 12/02/2008
  //============================================================
  void optout_status_redirect_scenario();
  
  //============================================================
  //scenario for testing correct OptOutFrontend redirection 
  //by getting invalid uid
  //============================================================
  void incorrect_uid_opt_out_scenario ();
 
  //============================================================
  //scenario for testing correct no cookies client matching
  //in the different colo cases
  //============================================================
  void client_without_cookes_scenario();

  //============================================================
  //scenario for testing cookies expiration on OO request
  //ce parameter used
  //============================================================
  void cookie_expiration();

};

#endif
