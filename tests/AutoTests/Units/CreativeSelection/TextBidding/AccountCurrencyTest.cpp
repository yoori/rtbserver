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

#include "AccountCurrencyTest.hpp"

REFLECT_UNIT(AccountCurrencyTest) (
  "CreativeSelection",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::SelectedCreativesCCID SelectedCreativesCCID;
  typedef AutoTest::SelectedCreativesActualCPC SelectedCreativesActualCPC;
  typedef AutoTest::DebugInfo::SelectedCreativesList SelectedCreativesList;
  typedef AutoTest::Money Money;
}

bool
AccountCurrencyTest::run_test()
{
  add_descr_phrase("ADSC-672");

  add_descr_phrase("Initializing");

  std::string keyword1(get_object_by_name("Keyword1").Value());
  const int tid1 = fetch_int("Tag1");

  std::vector<std::string> ccid_exp;
  std::vector<Money>       actual_cpc_exp;

  ccid_exp.push_back(fetch_string("CC3"));
  ccid_exp.push_back(fetch_string("CC4"));
  ccid_exp.push_back(fetch_string("CC2"));
  ccid_exp.push_back(fetch_string("CC1"));
  
  actual_cpc_exp.push_back(fetch_string("actual_cpc3"));
  actual_cpc_exp.push_back(fetch_string("actual_cpc4"));
  actual_cpc_exp.push_back(fetch_string("actual_cpc2"));
  actual_cpc_exp.push_back(fetch_string("actual_cpc1"));

  NSLookupRequest request;
  request.tid(tid1).referer_kw(keyword1);

  add_descr_phrase("Sending request");

  AdClient client(AdClient::create_user(this));

  // Send &referer_kw twice to make it a context word.
  client.process_request(request);
  client.process_request(request);

  FAIL_CONTEXT(
    AutoTest::sequence_checker(
      ccid_exp,
      SelectedCreativesCCID(client)).check(),
    "selected_creatives cc_id");
  FAIL_CONTEXT(
    AutoTest::sequence_checker(
      actual_cpc_exp,
      SelectedCreativesActualCPC(client)).check(),
    "selected_creatives actual_cpc");

  add_descr_phrase("Done");

  return true;
}
