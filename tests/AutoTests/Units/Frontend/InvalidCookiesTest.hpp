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


#ifndef __INVALIDCOOKIESTEST_HPP
#define __INVALIDCOOKIESTEST_HPP

#include <tests/AutoTests/Commons/Common.hpp>

/**
 * @class InvalidCookiesTest
 * @brief test for invalid cookies processing
 */
                                 
class InvalidCookiesTest: public BaseUnit
{
  
public:
 
  InvalidCookiesTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseUnit(stat_var, task_name, params_var),
    client(AutoTest::AdClient::create_user(this))
  {};
 
  virtual ~InvalidCookiesTest() throw()
  {};
 
private:
  AutoTest::AdClient client;
  AutoTest::NSLookupRequest ns_request;
  AutoTest::OptOutRequest   optout_request;
    
  virtual bool run_test();
  //test utils
  std::string crack_cookie(const char *cookie_name,
                           unsigned short pos,
                           const char* extStr,
                           bool replacing = true);
  void
  crack_cookie_and_check_serv_behaviour(const char* cookie_name,
                                        unsigned short pos,
                                        const char* extStr,
                                        bool replacing = true,
                                        bool check_host_cookie_presents = true);
  //test cases
  void set_up();
  
  void invalid_base64_uid_test_case();
  void invalid_uid_test_case();

  void invalid_last_colo_id_test_case();
  void opt_out_test_case();
  void invalid_opt_out_status_test_case();
  void opt_out_after_crack_test_case();

};

#endif  // __INVALIDCOOKIESTEST_HPP
