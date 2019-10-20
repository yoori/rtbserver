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

#include<cstdlib>
#include<iostream>
#include<Generics/Network.hpp>

unsigned resolve_index(const char* hostname)
{
  char buf[4096];
  hostent addresses;
  try
  {
    Generics::Network::Resolver::get_host_by_name(
      hostname,
      addresses,
      buf,
      sizeof(buf));
  }
  catch(const Generics::Network::Resolver::Exception& e)
  {
    std::cerr << "can't resolve hostname: " << hostname << ". : " << e.what();
    exit(1);
  }
  if(addresses.h_length>0)
  {
    return *reinterpret_cast<unsigned*>(addresses.h_addr_list[0]);
  }
  else
  {
    std::cerr << "can't get address for hostname: " << hostname;
    exit(1);
  }
}

int main(int argc, char* argv[])
{
  if(argc > 1)
  {
    unsigned  res = resolve_index(argv[1]);
    std::cout << "index = " << res << std::endl;
  }
  return 0;
}
