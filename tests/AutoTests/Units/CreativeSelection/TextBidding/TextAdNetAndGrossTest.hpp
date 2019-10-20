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


#ifndef _AUTOTEST__TEXTADVERTISINGNETANDGROSSTEST_
#define _AUTOTEST__TEXTADVERTISINGNETANDGROSSTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = AutoTest::ORM;

/**
 * @class TextAdNetAndGrossTest
 * @brief Test Net & gross campaigns behaviour
 * in the case of text advertising.
 */
class TextAdNetAndGrossTest: public BaseDBUnit
{
public:
 
  TextAdNetAndGrossTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~TextAdNetAndGrossTest() throw()
  {};
 
protected:
 
  virtual void pre_condition();
  virtual bool run();
  virtual void post_condition();
  virtual void tear_down();

  void case_tag_with_and_without_commission();
  void case_competition();
  void case_publisher_commission();

private:

  typedef ORM::StatsArray<ORM::HourlyStats, 6> Stats;
  Stats stats_;
  Generics::Time target_request_time_;
};
 
#endif //_AUTOTEST__TEXTADVERTISINGNETANDGROSSTEST_
