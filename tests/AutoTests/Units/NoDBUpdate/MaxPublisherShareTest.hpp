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


#ifndef _AUTOTEST__MAXPUBLISHERSHARETEST_
#define _AUTOTEST__MAXPUBLISHERSHARETEST_
 
#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = ::AutoTest::ORM;
 
class MaxPublisherShareTest: public BaseDBUnit
{
public:
 
  MaxPublisherShareTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var),
      check_count_(0)
  { }
 
  virtual ~MaxPublisherShareTest() throw()
  { }
 
private:

  int check_count_;
 
  void
  set_up();

  void
  tear_down();

  bool
  run();

  virtual
  void
  check() throw(eh::Exception);

  virtual
  bool
  checker_call(
    const std::string& description,
    AutoTest::Checker* checker)
    throw(eh::Exception);

  // Cases
  void
  share_expiring_();

  void
  increase_budget_();

  void
  increase_share_();

  void
  three_sites_part_1_(
    const AutoTest::Time& new_day);

  void
  three_sites_part_2_(
    const AutoTest::Time& new_day);

  void
  text_static_();

  void
  text_dynamic_();
  
  void
  text_daily_part_1_(
    const AutoTest::Time& new_day);

  void
  text_daily_part_2_(
    const AutoTest::Time& new_day);

  void
  text_daily_part_3_(
    const AutoTest::Time& new_day);

};

#endif //_AUTOTEST__MAXPUBLISHERSHARETEST_
