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


#include "SiteCreativeApprovalTest.hpp"

REFLECT_UNIT(SiteCreativeApprovalTest) (
  "CreativeSelection",
  AUTO_TEST_FAST);

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;

  struct TestCase
  {
    const char* tag;
    bool ad_shown;
  };

  const TestCase CASES[] =
  {
    { "TAG/1", false },
    { "TAG/2", true },
    { "TAG/3", false },
    { "TAG/4", false },
    { "TAG/5", true },
    { "TAG/6", false },
    { "TAG/7", false }, 
    { "TAG/8", false },
    { "TAG/9", true },
    { "TAG/10", false }
  };
  
}

bool
SiteCreativeApprovalTest::run_test()
{
  AdClient client(AdClient::create_user(this));

  for (size_t i = 0; i < countof(CASES); ++i)
  {
    client.process_request(
      NSLookupRequest().
        referer_kw(fetch_string("KWD")).
        tid(fetch_string(CASES[i].tag)));

    FAIL_CONTEXT(
      AutoTest::equal_checker(
        CASES[i].ad_shown? fetch_string("CC"): "0",
        client.debug_info.ccid).check(),
      "Check CC#" + strof(i+1));
  }
  return true;
}

