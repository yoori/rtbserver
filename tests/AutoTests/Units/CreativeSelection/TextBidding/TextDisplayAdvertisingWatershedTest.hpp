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

#ifndef _UNITTEST__TEXTDISPLAYADVERTISINGWATERSHEDTEST_
#define _UNITTEST__TEXTDISPLAYADVERTISINGWATERSHEDTEST_
 

#include <tests/AutoTests/Commons/Common.hpp>

/**
 * @class TextDisplayAdvertisingWatershedTest
 * @brief Test display advertising versus text advertising.
 */
 
class TextDisplayAdvertisingWatershedTest
  :public BaseUnit
{
public:
 
  TextDisplayAdvertisingWatershedTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~TextDisplayAdvertisingWatershedTest() throw()
  {};
 
private:
 
  virtual bool run_test();

  // Cases
  void
  display_wins_();

  void
  text_wins_();
 
};

#endif
