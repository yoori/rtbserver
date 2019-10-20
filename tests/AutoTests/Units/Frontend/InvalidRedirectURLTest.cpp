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

#include "InvalidRedirectURLTest.hpp"

REFLECT_UNIT(InvalidRedirectURLTest) (
  "Frontend",
  AUTO_TEST_FAST
);

namespace
{
  const char *const invalid_url[] =
  {
    "http://test\r\nSet-Cookie: test=1",
    "http:// ",
    "http://www.google .com/",
  };
}


static
void
test_request(AutoTest::AdClient &client,
             AutoTest::BaseRequest &request,
             int status)
{
  client.process(request, true);

  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      !client.has_cookie("test")),
    "cookie has been erroneously set");

  FAIL_CONTEXT(
    AutoTest::equal_checker(
      status,
      client.req_status()).check(),
    "must got expected status");
}


bool
InvalidRedirectURLTest::run_test()
{
  AutoTest::AdClient client(AutoTest::AdClient::create_user(this));

  for (size_t i = 0; i < sizeof(invalid_url) / sizeof(*invalid_url); ++i)
  {
    NOSTOP_FAIL_CONTEXT(
      {
        add_descr_phrase("Sending OO request");

        AutoTest::OptOutRequest oo_request;
        oo_request.op("status");
        oo_request.opted_in_url(invalid_url[i]).
          opted_out_url(invalid_url[i]).
          opt_undef_url(invalid_url[i]);
        test_request(client, oo_request, 200);
      },
      invalid_url[i]);
  }

  return true;
}
