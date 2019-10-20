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

#include "ForbiddenUserAgentsTest.hpp"

REFLECT_UNIT(ForbiddenUserAgentsTest) (
  "Frontend",
  AUTO_TEST_FAST
);

namespace
{
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
}

bool 
ForbiddenUserAgentsTest::run_test()
{
  const Locals locals = get_local_params();
  const size_t locals_size = locals.DataElem().size();
  
  AdClient client(AdClient::create_nonoptin_user(this));
  NSLookupRequest request;
  {
    request.user_agent = "Mozilla/5.0 (Windows; U; Windows NT 5.1;)";
    FAIL_CONTEXT(
      AutoTest::equal_checker(
        204,
        client.process(request, true)).check(),
      "must process valid user agent: "
      "Mozilla/5.0 (Windows; U; Windows NT 5.1;)");
  }
  for (size_t i = 2; i < locals_size; ++i)
  {
    const std::string &name  = locals.DataElem()[i].Name();
    const std::string &value = locals.DataElem()[i].Value();

    if (name.find("Agent") != std::string::npos)
    {
      request.user_agent = value;
    FAIL_CONTEXT(
      AutoTest::equal_checker(
        403,
        client.process(request, true)).check(),
      "must got forbidden status for agent: " + value);
    }
  }
  return true;
}
 
