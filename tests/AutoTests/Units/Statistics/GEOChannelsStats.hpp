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

#ifndef _AUTOTEST__GEOCHANNELSSTATS_
#define _AUTOTEST__GEOCHANNELSSTATS_
  
#include <tests/AutoTests/Commons/Common.hpp>

namespace DB = AutoTest::DBC;

class GEOChannelsStats : public BaseUnit
{
public:
  typedef AutoTest::AdClient AdClient;
  
public:
  GEOChannelsStats(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var),
    pq_conn_(open_pq())
  { }

  virtual ~GEOChannelsStats() throw()
  { }

private:

  virtual bool run_test();

  void make_requests();
  void make_location_request(
    AdClient& client,
    const char* tid,
    const std::string& location,
    const char* expected_ccids,
    unsigned short flags = 0);

  AutoTest::Time testtime;
  DB::Conn pq_conn_;
};

#endif // _AUTOTEST__GEOCHANNELSSTATS_
