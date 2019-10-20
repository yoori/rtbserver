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

#include "CombinedHardSoftMatchingTest.hpp"

REFLECT_UNIT(CombinedHardSoftMatchingTest) (
  "TriggerMatching",
  AUTO_TEST_FAST
);

typedef AutoTest::NSLookupRequest  NSLookupRequest;
typedef AutoTest::AdClient AdClient;
 
bool 
CombinedHardSoftMatchingTest::run_test()
{
  add_descr_phrase("XML data initializing");
  
  std::string required_channel = fetch_string("CombinedHardSoftMatchingTest/01");
 
  AdClient client(AdClient::create_user(this));
  
  NSLookupRequest request;
  request.tid = tid;
  request.referer = "google.com";

  add_descr_phrase("Sending requests for server validation");

  client.process_request(request.referer_kw("Test5 Test3 Test4"));
  
  FAIL_CONTEXT(
    AutoTest::entry_checker(
      required_channel, 
      client.debug_info.trigger_channels +
        client.debug_info.trigger_channels).check(),
    "must have required channel");

  client.process_request(request.referer_kw("Test5 Test4 Test3"));
  
  FAIL_CONTEXT(
    AutoTest::entry_checker(
      required_channel,
      client.debug_info.trigger_channels,
      AutoTest::SCE_NOT_ENTRY).check(),
    "must not have required channel in trigger_channels");
  
  FAIL_CONTEXT(
    AutoTest::entry_checker(
      required_channel,
      client.debug_info.trigger_channels,
      AutoTest::SCE_NOT_ENTRY).check(),
    "must not have required channel in trigger_channels");

  return true;
}
 
