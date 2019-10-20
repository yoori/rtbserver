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

#include "ChannelTargetingTest.hpp"

REFLECT_UNIT(ChannelTargetingTest) (
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
 
bool 
ChannelTargetingTest::run_test()
{
  NOSTOP_FAIL_CONTEXT(part1());
  NOSTOP_FAIL_CONTEXT(part2());
  return true;
}
 
void
ChannelTargetingTest::part1()
{
  std::string keyword   = fetch_string("Key1");
  std::string tid       = fetch_string("Tag1");
  std::string exp_ccid[] ={
    fetch_string("CC1"),
    fetch_string("CC2")
  };
  add_descr_phrase("Prepare server context part1");
  AdClient client(AdClient::create_user(this));
  AutoTest::NSLookupRequest request;
  request.tid = tid;
  request.referer_kw = keyword;
  client.process_request(request);
  client.process_request(request);

  FAIL_CONTEXT(
    AutoTest::sequence_checker(
      exp_ccid,
      SelectedCreativesCCID(client)).check(),
    "selected_creatives cc_ids");
}

void
ChannelTargetingTest::part2()
{
  std::string keyword   = fetch_string("Key2");
  std::string tid       = fetch_string("Tag2");
  std::string exp_ccid[] ={
    fetch_string("CC3"),
    fetch_string("CC4")
  };
  add_descr_phrase("Prepare server context part2");
  AdClient client(AdClient::create_user(this));

  AutoTest::NSLookupRequest request;
  request.tid = tid;
  request.referer_kw = keyword;
  client.process_request(request);
  client.process_request(request);
  
  FAIL_CONTEXT(
    AutoTest::sequence_checker(
      exp_ccid,
      SelectedCreativesCCID(client)).check(),
    "selected_creatives cc_ids");
}
