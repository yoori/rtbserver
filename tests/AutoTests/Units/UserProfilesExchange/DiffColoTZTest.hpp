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


#ifndef _AUTOTEST__DIFFCOLOTZTEST_
#define _AUTOTEST__DIFFCOLOTZTEST_
 
#include <tests/AutoTests/Commons/Common.hpp>
 
 
class DiffColoTZTest: public BaseUnit
{
public:
 
  DiffColoTZTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~DiffColoTZTest() throw()
  {};
 
private:

  double tz_ofset;
  int   colo_req_timeout;
  std::string remote1;
  std::string remote2;
  std::string colo1_id;
  std::string colo2_id;
  Generics::Time today;
  virtual bool run_test();

  void local_day_switch();
  void gmt_day_switch();
 
};

#endif //_AUTOTEST__DIFFCOLOTZTEST_
