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

#include "TextAdvertisingTest.hpp"

REFLECT_UNIT(TextAdvertisingTest) (
 "CreativeSelection",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::SelectedCreativesCCID SelectedCreativesCCID;
  typedef AutoTest::SelectedCreativesActualCPC SelectedCreativesActualCPC;
}

void 
TextAdvertisingTest::generic_scenario
(const AutoTest::NSLookupRequest &request,
 const std::vector<std::string> &ccid_exp,
 const std::vector<AutoTest::Money> &actual_cpc_exp)
{
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
}


bool 
TextAdvertisingTest::run_test()
{
  add_descr_phrase("ADSC-672");

  add_descr_phrase("Initializing");

  std::string keyword1(get_object_by_name("Keyword1").Value());
  std::string keyword2(get_object_by_name("Keyword2").Value());
  
  const int tid1 = fetch_int("Tag1");

  std::vector<std::string> ccid_exp;
  std::vector<AutoTest::Money> actual_cpc_exp;

  for (int i = 1; i <= 5; ++i)
  {
    std::string num = strof(i);
    ccid_exp.push_back(fetch_string("CC" + num));
    actual_cpc_exp.push_back(fetch_string("actual_cpc" + num));
  }

  AutoTest::NSLookupRequest request;
  request.tid(tid1);

  add_descr_phrase("scenario 1");
  // Scenario 1.
  request.referer_kw(keyword1);
  generic_scenario(request, ccid_exp, actual_cpc_exp);

  // Scenario 2.
  add_descr_phrase("scenario 2");
  ccid_exp.back() = fetch_string("CC6");
  actual_cpc_exp.back() = fetch_string("actual_cpc6");

  request.referer_kw = keyword1 + "," + keyword2;
  generic_scenario(request, ccid_exp, actual_cpc_exp);

  add_descr_phrase("Done");

  return true;
}
