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


#include "InactiveCCGTest.hpp"
 
REFLECT_UNIT(InactiveCCGTest) (
  "NoDBUpdate",
  AUTO_TEST_QUIET
);

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::CampaignChecker CampaignChecker;
}
 
bool 
InactiveCCGTest::run_test()
{
  unsigned long ccgid = fetch_int("IC_CCGID");

  FAIL_CONTEXT(CampaignChecker(
      this,
      ccgid,
      CampaignChecker::Expected().status("I").eval_status("A")).check(),
    "Inactive CCG");

  AdClient client = AdClient::create_user(this);

  client.process_request(NSLookupRequest().
    referer_kw(fetch_string("KEYWORD")).
    tid(fetch_string("IC_TID")).
    debug_time(Generics::Time::get_time_of_day()));

  FAIL_CONTEXT(
    AutoTest::equal_checker(
      "0",
      client.debug_info.ccid).check(),
    "mustn't return ccid for inactive CCG");

  return true;
}

