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


#include "AccountBudgetTest.hpp"
 
REFLECT_UNIT(AccountBudgetTest) (
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
AccountBudgetTest::run_test()
{
  NSLookupRequest request;
  request.referer_kw(fetch_string("KEYWORD"));
  request.tid(fetch_string("ZEROBALANCE_TID"));
  request.debug_time(Generics::Time::get_time_of_day());
  unsigned long ccgid = fetch_int("ZEROBALANCE_CCGID");

  // ecpm = 501$, budget = 1$
  // after 2 requests, we have reaching zero balance
  unsigned long max_repeat_count =
    (fetch_int("ZEROBALANCE_ACCOUNT_CREDIT") * 1000 /
     fetch_int("ZEROBALANCE_CPM")) + 1; 
  
  // Check initial state
  FAIL_CONTEXT(CampaignChecker(
      this,
      ccgid,
      CampaignChecker::Expected().eval_status("A").status("A")).check(),
    "zero balance initial state");

  for (unsigned long i = 0; i < max_repeat_count; ++i)
  {
    AdClient client = AdClient::create_user(this);
    client.process_request(request);
    // Check only first request
    if (i)
    {
        FAIL_CONTEXT(
          AutoTest::equal_checker(
            fetch_string("ZEROBALANCE_CCID"),
            client.debug_info.ccid).check(),
          "must return zero balance ccid");
    }
  }

  FAIL_CONTEXT(AutoTest::wait_checker(
    CampaignChecker(
      this,
      ccgid,
      CampaignChecker::Expected().eval_status("I"))).check(),
    "zero balance after reaching budget");

  AdClient client = AdClient::create_user(this);
  client.process_request(request);
  FAIL_CONTEXT(
    AutoTest::equal_checker(
      "0",
      client.debug_info.ccid).check(),
    "mustn't return zero balance ccid");

  return true;
}

