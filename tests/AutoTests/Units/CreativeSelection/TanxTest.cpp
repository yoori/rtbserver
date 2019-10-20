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


#include "TanxTest.hpp"
#include <fstream>

REFLECT_UNIT(TanxTest) (
  "CreativeSelection",
  AUTO_TEST_FAST);

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::TanxRequest TanxRequest;
  typedef AutoTest::TanxResponseChecker TanxResponseChecker;
}

bool
TanxTest::run_test()
{
  AUTOTEST_CASE(
    base_(),
    "Base case");

  return true;
}

void
TanxTest::base_()
{
  AdClient client(AdClient::create_user(this));

  client.process_request(NSLookupRequest());

  // to avoid min_RTB_user_age check
  client.process_post(
    TanxRequest().
      debug_time(AutoTest::Time() - 24*60*60));

  TanxRequest request;
  request.
    aid(fetch_int("TANX/ACCOUNT")).
    debug_ccg(fetch_int("BASE/CCG")).
    size(fetch_string("BASE/SIZE")).
    ad_bid_count(1).
    min_cpm_price(1);
 
  client.process_post(request);

  FAIL_CONTEXT(
    TanxResponseChecker(client,
      TanxResponseChecker::Expected().
        creative_id(fetch_string("BASE/CREATIVE")).
        destination_url("http://www.autotest.com/").
        ad_bid_count_idx(0)).check(),
    "Tanx response creative");
}
