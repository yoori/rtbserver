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

#include "SoftTriggerMatchingTest.hpp"
#include <String/InterConvertion.hpp>

REFLECT_UNIT(SoftTriggerMatchingTest) (
  "TriggerMatching",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::NSLookupRequest  NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::ChannelsCheck ChannelsCheck;

  struct TestCase
  {
    const char* referer_kw;
    const char* matched;
    const char* unmatched;
  };

  const TestCase CASES[] =
  {
    {
      "Test1 Test2",
      "Channel/01",
      0
    },
    {
      "Test2 Test1",
      "Channel/01",
      0
    },
    {
      "Test2 Test3 Test1",
      "Channel/01",
      0
    },
    {
      "Test1",
      0,
      "Channel/01"
    },
    {
      "Test2",
      0,
      "Channel/01"
    },
    {
      "Test17 Test16 Test15 Test21 "
      "Test18 Test20 Test19",
      "Channel/02,Channel/03",
      0
    },
    {
      "Test17 Test16 Test15 Test18 "
      "Test18 Test20 Test19",
      "Channel/02",
      "Channel/03"
    }
  };
  
}
 
bool 
SoftTriggerMatchingTest::run_test()
{
  AdClient client(AdClient::create_user(this));

  for (unsigned int i=0; i < countof(CASES); ++i)
  {
    NSLookupRequest request;
    request.referer_kw = CASES[i].referer_kw;
    client.process_request(request);

    if (CASES[i].matched)
    {
      FAIL_CONTEXT(
        ChannelsCheck(
          this,
          CASES[i].matched,
          client.debug_info.trigger_channels).check(),
        "Expected history#" + strof(i+1));
    }
    if (CASES[i].unmatched)
    {
      FAIL_CONTEXT(
        ChannelsCheck(
          this,
          CASES[i].unmatched,
          client.debug_info.trigger_channels,
          AutoTest::SCE_NOT_ENTRY).check(),
        "Unexpected history#" + strof(i+1));
    }
  }

  return true;
}
 
