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


#include "ExactUrlMatching.hpp"
 
REFLECT_UNIT(ExactUrlMatching) (
  "TriggerMatching",
  AUTO_TEST_FAST
);

namespace {
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::ChannelsCheck ChannelsCheck;

  const AutoTest::SequenceCheckerEnum REQUESTS[] =
  {
    AutoTest::SCE_ENTRY,     // 1
    AutoTest::SCE_ENTRY,     // 2
    AutoTest::SCE_ENTRY,     // 3
    AutoTest::SCE_ENTRY,     // 4
    AutoTest::SCE_ENTRY,     // 5
    AutoTest::SCE_ENTRY,     // 6
    AutoTest::SCE_ENTRY,     // 7
    AutoTest::SCE_NOT_ENTRY, // 8
    AutoTest::SCE_NOT_ENTRY, // 9
    AutoTest::SCE_ENTRY,     // 10
    AutoTest::SCE_NOT_ENTRY, // 11
    AutoTest::SCE_ENTRY,     // 12
    AutoTest::SCE_ENTRY,     // 13
    AutoTest::SCE_ENTRY,     // 14
    AutoTest::SCE_ENTRY,     // 15
    AutoTest::SCE_ENTRY      // 16
  };
}

 
bool 
ExactUrlMatching::run_test()
{
  AdClient client(AdClient::create_user(this));

  for (unsigned int i = 0; i < sizeof(REQUESTS) / sizeof(*REQUESTS); ++i)
  {
    add_descr_phrase(("Request " + strof(i+1)).c_str());
    std::string ref_name = "REF" + strof(i+1);
    NSLookupRequest request;
    request.referer = fetch_string(ref_name);
    client.process_request(request);

    FAIL_CONTEXT(
      ChannelsCheck(
        this,
        "Channel",
        client.debug_info.trigger_channels,
        REQUESTS[i]).check(),
      "Check trigger_channels#" +  strof(i+1));
    
  }
  return true;
}

