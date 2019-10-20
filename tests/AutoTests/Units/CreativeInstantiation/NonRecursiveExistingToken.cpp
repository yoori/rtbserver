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


#include "NonRecursiveExistingToken.hpp"
 
REFLECT_UNIT(NonRecursiveExistingToken) (
  "CreativeInstantiation",
  AUTO_TEST_FAST
);

namespace {
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::CreativeChecker CreativeChecker;
}
 
bool 
NonRecursiveExistingToken::run_test()
{

  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      get_config().check_service(CTE_ALL, STE_CAMPAIGN_MANAGER)),
    "CampaignManager must prisent in the configuration file");
   
  add_descr_phrase("Ensure that Creative#2 (cc_id=" +
                   fetch_string("CC2") +
                   ") status 'C'");

  FAIL_CONTEXT(
    CreativeChecker(
      this,
      fetch_int("CC2"),
      CreativeChecker::Expected().
        status("C")).check(),
    "Check creative#2 status");

  add_descr_phrase("Ensure that Creative#1 (cc_id=" +
                   fetch_string("CC1") +
                   ") status 'A'");
  FAIL_CONTEXT(
    CreativeChecker(
      this,
      fetch_int("CC1"),
      CreativeChecker::Expected().
        status("A")).check(),
    "Check creative#1 status");

  add_descr_phrase("Check that Creative with high "
                   "weight doesn't match");
  NSLookupRequest request;
  request.referer_kw = fetch_string("KEYWORD");
  request.tid = fetch_string("TAG");

  AdClient client(AdClient::create_user(this));
  
  client.process_request(request);
  FAIL_CONTEXT(
    AutoTest::equal_checker(
      fetch_string("CC1"),
      client.debug_info.ccid).check(),
    "must return creative#1");

  return true;
}

