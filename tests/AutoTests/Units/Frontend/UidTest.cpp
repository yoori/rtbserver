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

#include "UidTest.hpp"

REFLECT_UNIT(UidTest) (
  "Frontend",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
}

void
UidTest::uid_installing()
{
  add_descr_phrase("UID installing");
  AdClient client(AdClient::create_nonoptin_user(this));
  
  client.process_request(
    NSLookupRequest().setuid(1));

  FAIL_CONTEXT(
    AutoTest::equal_checker(
      AutoTest::PROBE_UID,
      client.get_uid(),
      AutoTest::CT_NOT_EQUAL).check(),
    "Got unexpected PROBE uid");
}

void
UidTest::probe_uid()
{
  add_descr_phrase("Probe UID case");

  {
    AdClient client(AdClient::create_nonoptin_user(this));
    
    client.set_probe_uid();
    
    client.process_request(
      NSLookupRequest().setuid(1));

    FAIL_CONTEXT(
      AutoTest::equal_checker(
        AutoTest::PROBE_UID,
        client.get_uid(),
        AutoTest::CT_NOT_EQUAL).check(),
      "Got unexpected PROBE uid");
  }

  {
    AdClient client(AdClient::create_undef_user(this));

    std::string uid = client.get_uid();
  
    client.process_request(
      NSLookupRequest());
       
    FAIL_CONTEXT(
      AutoTest::equal_checker(
        uid,
        client.get_uid()).check(),
      "Got unexpected uid");

    client.process_request(
      NSLookupRequest().setuid(1));

    FAIL_CONTEXT(
      AutoTest::equal_checker(
        uid,
        client.get_uid()).check(),
      "Got unexpected uid");
  }
}

bool 
UidTest::run_test()
{
  NOSTOP_FAIL_CONTEXT(uid_installing());
  NOSTOP_FAIL_CONTEXT(probe_uid());
  return true;
}
