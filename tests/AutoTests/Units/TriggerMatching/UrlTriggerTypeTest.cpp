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


#include "UrlTriggerTypeTest.hpp"
 
REFLECT_UNIT(UrlTriggerTypeTest) (
  "TriggerMatching",
  AUTO_TEST_FAST
);

namespace {
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::DebugInfoList    DebugInfoList;

  const unsigned int REFERER_SIZE = 7;
  const unsigned int CHANNEL_SIZE = 11;

  bool channel_table[REFERER_SIZE][CHANNEL_SIZE] = {
    { 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 },
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
    { 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0 },
    { 1, 1, 0, 1, 1, 1, 0, 1, 1, 0, 0 }
  };
      
}

bool 
UrlTriggerTypeTest::run_test()
{
  AdClient client(AdClient::create_user(this));
  for (unsigned int req_idx = 0; req_idx < REFERER_SIZE; ++req_idx)
  {
    add_descr_phrase(("Request " + strof(req_idx+1)).c_str());
    std::string ref_name = "REF" + strof(req_idx+1);
    NSLookupRequest request;
    request.referer = fetch_string(ref_name);
    DebugInfoList matched, unmatched;
    for (unsigned int channel_idx = 0; channel_idx < CHANNEL_SIZE; ++channel_idx)
    {
      std::string channel_name = "Channel" + strof(channel_idx+1);
      if (channel_table[req_idx][channel_idx])
      {
        matched.push_back(fetch_string(channel_name));
      }
      else
      {
        unmatched.push_back(fetch_string(channel_name));
      }
    }

    client.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        unmatched,
        client.debug_info.trigger_channels, 
        AutoTest::SCE_NOT_ENTRY).check(),
      "channels shouldn't match");
    
    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        matched,
        client.debug_info.trigger_channels, 
        AutoTest::SCE_ENTRY).check(),
      "page channel should match");
  }
  return true;
}

