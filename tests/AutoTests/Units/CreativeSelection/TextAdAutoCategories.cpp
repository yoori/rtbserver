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

#include "TextAdAutoCategories.hpp"

REFLECT_UNIT(TextAdAutoCategories) (
  "CreativeSelection",
  AUTO_TEST_FAST
);

bool
TextAdAutoCategories::run_test()
{
  std::vector<std::string> ccid_exp;
  std::vector<AutoTest::Money> actual_cpc_exp;

  const int total_creatives = fetch_int("TotalCreatives");
  for (int i = 0; i < total_creatives; ++i)
  {
    ccid_exp.push_back(fetch_string("CC" + strof(i + 1)));
    actual_cpc_exp.push_back(fetch_string("ActualCPC" + strof(i + 1)));
  }

  AutoTest::NSLookupRequest request;
  request.tid = fetch_string("Tag1");
  request.referer_kw = fetch_string("Keyword1");

  AutoTest::AdClient client(AutoTest::AdClient::create_user(this));
  client.process_request(request);
  client.process_request(request);

  FAIL_CONTEXT(
    AutoTest::sequence_checker(
      ccid_exp,
      AutoTest::SelectedCreativesCCID(client)).check(),
    "selected_creatives cc_id");
  
  FAIL_CONTEXT(
    AutoTest::sequence_checker(
      actual_cpc_exp,
      AutoTest::SelectedCreativesActualCPC(client)).check(),
    "selected_creatives actual_cpc");

  return true;
}
