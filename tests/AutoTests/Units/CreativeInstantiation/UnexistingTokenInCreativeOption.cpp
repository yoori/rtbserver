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


#include "UnexistingTokenInCreativeOption.hpp"
 
REFLECT_UNIT(UnexistingTokenInCreativeOption) (
  "CreativeInstantiation",
  AUTO_TEST_SLOW
);

namespace {
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::CreativeChecker CreativeChecker;
  typedef AutoTest::ORM::PQ::CreativeOptionValue CreativeOptionValue;
}


void UnexistingTokenInCreativeOption::set_up()
{

  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      get_config().check_service(CTE_ALL, STE_CAMPAIGN_MANAGER)),
    "CampaignManager must prisent in the configuration file");

  add_descr_phrase("Preconditions");
  set_creative_option();
}

void
UnexistingTokenInCreativeOption::tear_down()
{
  CreativeOptionValue option(pq_conn_);
  option.select(fetch_int("CC1"),
                fetch_int("OPTION"));
  option.value = "##FUDJIN##";
  option.update();
}

bool
UnexistingTokenInCreativeOption::run()
{
  add_descr_phrase("Test 1.1. Static test");
  NSLookupRequest request;
  request.referer_kw = fetch_string("KEYWORD");
  request.tid = fetch_string("TAG");

  {
    AdClient client(AdClient::create_user(this));
    
    client.process_request(request);
    FAIL_CONTEXT(
      AutoTest::equal_checker(
        fetch_string("CC2"),
        client.debug_info.ccid).check(),
      "must return creative#2");
  }

  add_descr_phrase("Test 1.2. Dynamic test - "
                   "changing Creative Option "
                   "with version updating");

  CreativeOptionValue option(pq_conn_);
  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      option.select(
        fetch_int("CREATIVE1"),
        fetch_int("OPTION"))),
    "Can't select creative option value");
  option.value.null();
  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      option.update()),
    "Can't update creative option value");

  FAIL_CONTEXT(
    AutoTest::wait_checker(
      CreativeChecker(
        this,
        fetch_int("CC1"),
        CreativeChecker::Expected().
          status("A"))).check(),
    "Check creative#1 status");  
  
  {
    AdClient client(AdClient::create_user(this));
    
    client.process_request(request);
    FAIL_CONTEXT(
      AutoTest::equal_checker(
        fetch_string("CC1"),
        client.debug_info.ccid).check(),
      "must return creative#1");
  }

  add_descr_phrase("Postcondition");
  set_creative_option();

  return true;
}


void UnexistingTokenInCreativeOption::set_creative_option()
{
  CreativeOptionValue option(pq_conn_);
  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      option.select(
        fetch_int("CREATIVE1"),
        fetch_int("OPTION"))),
    "Can't select creative option value");
  option.value = "##FUDJIN##";
  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      option.update()),
    "Can't update creative option value");

  FAIL_CONTEXT(
    AutoTest::wait_checker(
      CreativeChecker(
        this,
        fetch_int("CC1"),
        CreativeChecker::Expected().
          status("C"))).check(),
    "Check creative#1 status");

  FAIL_CONTEXT(
    AutoTest::wait_checker(
      CreativeChecker(
        this,
        fetch_int("CC2"),
        CreativeChecker::Expected().
          status("A"))).check(),
      "Check creative#2 status");
}
 

