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

#include <eh/Exception.hpp>
#include <iostream>
#include <Commons/ExternalUserIdUtils.hpp>

int
main(int argc, char* argv[])
{
  if(argc < 2)
  {
    std::cerr << "argument not defined" << std::endl;
    return 1;
  }

  AdServer::Commons::ExternalUserIdArray user_ids;

  AdServer::Commons::dns_decode_external_user_ids(
    user_ids,
    String::SubString(argv[1]));

  for(auto it = user_ids.begin(); it != user_ids.end(); ++it)
  {
    std::cout << *it << std::endl;
  }

  return 0;
}

