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


#include "BaiduTest.hpp"

REFLECT_UNIT(BaiduTest) (
  "CreativeSelection",
  AUTO_TEST_FAST);

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::BaiduRequest BaiduRequest;
  typedef AutoTest::UserBindRequest UserBindRequest;
  typedef AutoTest::BaiduResponseChecker BaiduResponseChecker;
}


void
BaiduTest::set_up()
{ }

void
BaiduTest::tear_down()
{ }

bool
BaiduTest::run()
{
  AUTOTEST_CASE(
    base_(),
    "Base case");

  return true;
}

void
BaiduTest::base_()
{
  AdClient rtb(AdClient::create_user(this));

  std::string tid(rtb.get_uid());

  rtb.process_request(
    UserBindRequest().tid(tid));

  // to avoid min_RTB_user_age check
  rtb.process_post(
    BaiduRequest().
      id(tid).
      debug_time(now_ - 24*60*60));
 
  rtb.process_post(
    BaiduRequest().
      aid(fetch_int("BASE/ACCOUNT")).
      debug_ccg(fetch_int("BASE/CCG")).
      id(tid).
      url(fetch_string("BASE/URL")).
      debug_size(fetch_string("SIZE")).
      debug_time(now_).
      minimum_cpm(0));

  FAIL_CONTEXT(
    BaiduResponseChecker(rtb,
      BaiduResponseChecker::Expected().
        advertiser_id(fetch_int("BASE/ADV")).
        creative_id(fetch_int("BASE/CREATIVE"))).check(),
    "Baidu response creative");
}


