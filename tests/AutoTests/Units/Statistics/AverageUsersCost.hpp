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

#ifndef _AUTOTEST__AVERAGEUSERSCOST_
#define _AUTOTEST__AVERAGEUSERSCOST_
  
#include <tests/AutoTests/Commons/Common.hpp>

class AverageUsersCost : public BaseUnit
{
public:
  AverageUsersCost(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var),
    pq_conn_(open_pq())
  {};

  virtual ~AverageUsersCost() throw()
  {};

  struct UserRequest
  {
    const char* tag;
    const char* ccid;
    const char* revenue;
    bool track_imp;
    unsigned char actions;
  };

private:

  AutoTest::DBC::Conn pq_conn_;
  AutoTest::Time today_;

  std::string request_keyword_;
  unsigned long request_channel_;
  unsigned long request_k_channel_;
  unsigned long default_colo_;

  std::list<std::string> uids_;

  void
  log_profile(
    std::string uid);

  template<size_t RequestsCount>
  double
  test_case(
    const std::string& description,
    unsigned long test_channel,
    const char* test_keyword,
    const UserRequest(&requests) [RequestsCount],
    unsigned long flags = 0);

  virtual
  bool
  run_test();

};

#endif // _AUTOTEST__AVERAGEUSERSCOST_
