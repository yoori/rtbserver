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

#ifndef _UNITTEST__USERINFOEXCHANGERFUNCTIONALITYTEST_
#define _UNITTEST__USERINFOEXCHANGERFUNCTIONALITYTEST_

#include <tests/AutoTests/Commons/Common.hpp>
 
class UserInfoExchangerFunctionalityTest:
  public BaseUnit
{
  typedef AutoTest::AdClient AdClient;
  struct CheckRequest
  {
    AdClient client;
    std::string request;
    std::string expected_colo;
    std::list<std::string> expected_history_channels;
    std::string description;
    bool exists;

    CheckRequest(const AdClient& client,
                 const char* request,
                 const char* colo,
                 const char* channel,
                 const char* description,
                 bool exists = true);

    CheckRequest(const AdClient& client,
                 const char* request,
                 const char* colo,
                 const char** channels,
                 size_t channels_size,
                 const char* description,
                 bool exists = true);
  };

  typedef std::list<CheckRequest> CheckRequests;
  
public:
 
  UserInfoExchangerFunctionalityTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var): 
    BaseUnit(stat_var, task_name, params_var)
  {};

  virtual ~UserInfoExchangerFunctionalityTest() throw()
  {};
 
private:
  std::string s_channel;
  std::string ht_channel;
  std::string h_channel;

  std::string s_bp;
  std::string ht_bp;
  std::string h_bp;

  std::string s_keyword;
  std::string ht_keyword;
  std::string h_keyword;

  std::string major_frontend_prefix;
  std::string minor_frontend_prefix;

  unsigned long colo_req_timeout;

  std::string colo1_id;
  std::string colo2_id;

  CheckRequests requests;
 
  virtual bool run_test();

  void part1();
  void part2();
  void part3();
  void part4();
  void part5_colo1(AdClient& client);
  void part5_colo2(AdClient& client);
  void part5_return_to_colo1(AdClient& client);
  
  void merging_profiles_wait(AdClient& client,
                             const char* frontend_dst,
                             const char* colo_dst,
                             const char* marker_channel_name);
  void verification();
 
};

#endif // _UNITTEST__USERINFOEXCHANGERFUNCTIONALITYTEST_

