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

#ifndef COMMONS_ZMQCONFIG_H_
#define COMMONS_ZMQCONFIG_H_

#include <string>

#include <eh/Exception.hpp>

#include <xsd/AdServerCommons/AdServerCommons.hpp>
#include <Commons/zmq.hpp>

namespace Config
{
  class ZmqConfigReader
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  public:
    static
    int
    get_socket_type(const std::string& str_type)
      throw (Exception);

    static
    void
    set_socket_params(
      const xsd::AdServer::Configuration::ZmqSocketType& socket_config,
      zmq::socket_t& socket)
      throw (zmq::error_t);

    static
    std::string
    get_address(const xsd::AdServer::Configuration::ZmqAddressType& address_config)
      throw (eh::Exception);
  };
}

#endif /* COMMONS_ZMQCONFIG_H_ */
