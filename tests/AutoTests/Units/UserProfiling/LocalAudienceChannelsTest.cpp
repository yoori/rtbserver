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


#include "Common.hpp"
#include "LocalAudienceChannelsTest.hpp"

REFLECT_UNIT(LocalAudienceChannelsTest) (
  "UserProfiling",
  AUTO_TEST_SLOW);

namespace
{
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::TemporaryAdClient TemporaryAdClient;
  typedef AutoTest::ChannelsCheck ChannelsCheck;

  const char TEST_1_1[] = "Persistent UID matching";
}

void LocalAudienceChannelsTest::persistent_uid_matching_setup_()
{
  
}

void LocalAudienceChannelsTest::persistent_uid_matching_()
{

}

void LocalAudienceChannelsTest::stage1()
{
  AUTOTEST_CASE(persistent_uid_matching_setup_(), TEST_1_1);
  persistent_uid_matching_();
}

void LocalAudienceChannelsTest::stage2()
{
  AUTOTEST_CASE(persistent_uid_matching_(), TEST_1_1);
}

bool
LocalAudienceChannelsTest::run_test()
{
  stage1();
  check();
  stage2();
  check();
  return true;
}
