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


#include "FreqCapMergingTest.hpp"
#include <Generics/Uuid.hpp>

REFLECT_UNIT(FreqCapMergingTest) (
  "CreativeSelection",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::TemporaryAdClient TemporaryAdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;

  // Frequency caps enabled on merge.
  FreqCapMergingTest::TestRequest FC_ENABLED_ON_MERGE[] =
  {
    {
      0,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, "CC"
    },
    {
      1,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, 0
    },
    {
      2,
      FreqCapMergingTest::RE_TEMPORARY,
      "KEYWORD", "Tag",
      "Channel", 0
    },
    {
      3,
      FreqCapMergingTest::RE_MERGE,
      "KEYWORD", "Tag",
      0, 0
    },
    {
      4,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, 0
    },
    {
      6,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, "CC"
    },
    {
      7,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, 0
    }
  };

  // Frequency caps expired on merge.
  FreqCapMergingTest::TestRequest FC_EXPIRED_ON_MERGE[] =
  {
    {
      0,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, "CC"
    },
    {
      1,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, 0
    },
    {
      2,
      FreqCapMergingTest::RE_TEMPORARY,
      "KEYWORD", "Tag",
      "Channel", 0
    },
    {
      6,
      FreqCapMergingTest::RE_MERGE,
      "KEYWORD", "Tag",
      0, "CC"
    },
    {
      7,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, 0
    },
    {
      12,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, "CC"
    },
    {
      13,
      FreqCapMergingTest::RE_PERSISTENT,
      "KEYWORD", "Tag",
      0, 0
    }
  };
}

bool 
FreqCapMergingTest::run_test()
{

  NOSTOP_FAIL_CONTEXT(
    check(
      "Frequency caps enabled on merge.",
      FC_ENABLED_ON_MERGE));

  NOSTOP_FAIL_CONTEXT(
    check(
      "Frequency caps expired on merge.",
      FC_EXPIRED_ON_MERGE));
  
  return true;
}

template <size_t COUNT>
void FreqCapMergingTest::check(
  const std::string& description,
  const TestRequest (&requests)[COUNT])
{
  add_descr_phrase(description);
  AdClient pclient(AdClient::create_user(this));
  TemporaryAdClient tclient(
    TemporaryAdClient::create_user(this));
  
  AutoTest::Time time;
  for (unsigned long i=0; i < COUNT; ++i)
  {
    AdClient* client =
      requests[i].request_type == RE_TEMPORARY?
      &tclient: &pclient;
    NSLookupRequest request;
    if (requests[i].tid)
    {
      request.tid = fetch_string(requests[i].tid);
    }
    request.referer_kw =
      fetch_string(requests[i].referer_kw);
    request.debug_time = time + requests[i].time_ofset;

    if (requests[i].request_type == RE_MERGE)
    {
      client->merge(tclient, request);
    }
    else
    {
      client->process_request(request);
    }

    if (requests[i].expected_history)
    {
      FAIL_CONTEXT(
        AutoTest::entry_checker(
          fetch_string(requests[i].expected_history),
          client->debug_info.history_channels).check(),
        description +
        " History channels check#" + strof(i));
    }

    std::string expected_ccid = requests[i].expected_ccid?
      fetch_string(requests[i].expected_ccid): "0";      

    FAIL_CONTEXT(
      AutoTest::equal_checker(
        expected_ccid,
        client->debug_info.ccid).check(),
      description +
        " Check ccid#" +strof(i));    
  }
}

