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

#ifndef _AUTOTEST__GEOCHANNELSMATCHING_
#define _AUTOTEST__GEOCHANNELSMATCHING_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class GEOChannelsMatching : public BaseUnit
{
public:
  typedef AutoTest::NSLookupRequest NSLookupRequest;

  struct LocTestRequest
  {
    std::string description;
    const char* location;
    const char* expected;
    const char* unexpected;
    NSLookupRequest::Member param;
  };

  struct IPTestRequest
  {
    std::string description;
    const char* ip;
    const char* expected_location;
    const char* expected_channels;
  };

  
public:
  GEOChannelsMatching(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var)
  {};

  virtual ~GEOChannelsMatching() throw()
  {};

private:

  virtual bool
  run_test();

  void
  location_name_case(
    const LocTestRequest& testcase);

  void
  ip_case(
    const IPTestRequest& testcase);

};

#endif // _AUTOTEST__GEOCHANNELSMATCHING_
