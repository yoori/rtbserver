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

#include "TokenPrefixesTest.hpp"
#include <String/UTF8Case.hpp>

REFLECT_UNIT(TokenPrefixesTest) (
  "CreativeInstantiation",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::NSLookupRequest NSLookupRequest;
}

void TokenPrefixesTest::checkBody(
  const AdClient& client,
  const char* exp_body_tag)
{
 std::string expBody;
 std::string gotBody;
 String::case_change<String::Lower>(
   fetch_string(exp_body_tag),
   expBody);
 String::case_change<String::Lower>(
   client.req_response_data(),
   gotBody);
 unsigned long expLen = expBody.length();
 unsigned long gotLen = gotBody.length();
 FAIL_CONTEXT(
   AutoTest::equal_checker(
     expLen,
     gotLen).check(),
   "len of body must be expected");
 FAIL_CONTEXT(
   AutoTest::equal_checker(
     expBody,
     gotBody).check(),
   "must got expected body");
}
 
bool 
TokenPrefixesTest::run_test()

{
  tokenPrefixesSimpleFileCase();
  return true;
}

void TokenPrefixesTest::tokenPrefixesSimpleFileCase()
{
  add_descr_phrase("'TokenPrefixes.SimpleFile' test running");
  AdClient client(AdClient::create_user(this));
  NSLookupRequest request;
  request.tid = fetch_string("Tag/01");
  request.referer_kw = fetch_string("Keyword/01");
  client.process_request(request);
  checkBody(client, "Expected Body/01");
}


 
