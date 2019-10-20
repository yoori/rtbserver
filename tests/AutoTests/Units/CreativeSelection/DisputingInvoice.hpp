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


#ifndef _AUTOTEST__DISPUTINGINVOICE_
#define _AUTOTEST__DISPUTINGINVOICE_
  
#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = ::AutoTest::ORM;
namespace DB  = ::AutoTest::DBC;

class DisputingInvoice : public BaseDBUnit
{
public:
  DisputingInvoice(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseDBUnit(stat_var, task_name, params_var)
  { }

  virtual ~DisputingInvoice() throw()
  { }

private:

  int account_;
  int ccgid_;
  int ccid_;
  unsigned long tid_;
  std::string keyword_;
  ORM::HourlyStats stat;

  void
  set_up();

  void
  tear_down();

  bool
  run();

  // Cases
  void
  create_invoice_(
    ORM::PQ::Accountfinancialdata* acc_data);
  
  void
  edit_invoice_(
    ORM::PQ::Accountfinancialdata* acc_data);

  // Utils
  void
  clear_stats_();
  
};

#endif // _AUTOTEST__DISPUTINGINVOICE_

