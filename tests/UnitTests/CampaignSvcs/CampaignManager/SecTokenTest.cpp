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

#include <arpa/inet.h>
#include <netinet/in.h>

#include "../../TestHelpers.hpp"

#include <CampaignSvcs/CampaignManager/SecToken.hpp>

using namespace AdServer::CampaignSvcs;

class DependencyContainerMock : public DependencyContainer
{
public:
  Generics::Time now;
  uint32_t random_value;

public:
  virtual Generics::Time
  get_time_of_day() const throw ()
  {
    return now;
  }

  virtual uint32_t
  safe_rand() const throw ()
  {
    return random_value;
  }

protected:
  virtual
  ~DependencyContainerMock() throw ()
  {}
};

typedef ReferenceCounting::SmartPtr<DependencyContainerMock>
  DependencyContainerMock_var;

TEST(get_text_token)
{
  SecTokenGenerator::Config config;
  config.aes_keys.push_back("MFBOeuplH3LlQqfGSvNiew==");
  config.aes_keys.push_back("gYi+K845gfiUkvx7Se1zKA==");
  config.aes_keys.push_back("mVa0GF8y1uvou3mktkNxqw==");
  config.aes_keys.push_back("mCFIshSwVu4VxRTi73Rf+Q==");
  config.aes_keys.push_back("fCW/qXof04/U/qRTggbJLw==");
  config.aes_keys.push_back("K2UPkfYBFNA+MxHKkV5OUA==");
  config.aes_keys.push_back("emcqWycuiztgJVng4az0Xw==");

  DependencyContainerMock_var dependency_container =
    new DependencyContainerMock();
  config.custom_dependency_container = dependency_container;
  dependency_container->now =
    Generics::ExtendedTime(2014, 1, 27, 13, 55, 58, 0);
  dependency_container->random_value = 16395523;

  SecTokenGenerator_var token_generator = new SecTokenGenerator(config, 0);

  {
    uint32_t addr;
    ASSERT_TRUE (inet_pton(AF_INET, "127.0.0.1", &addr) > 0);
    ASSERT_EQUALS (token_generator->get_text_token(addr), "RCDulVH4ge2UajX2mze8AAAB");

    ASSERT_TRUE (inet_pton(AF_INET, "127.0.0.2", &addr) > 0);
    ASSERT_FALSE (token_generator->get_text_token(addr) == "RCDulVH4ge2UajX2mze8AAAB");
  }
}

RUN_TESTS
