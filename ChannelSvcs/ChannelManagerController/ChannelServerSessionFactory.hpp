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

#ifndef CHANNELSERVERSESSIONFACTORY_HPP
#define CHANNELSERVERSESSIONFACTORY_HPP

#include <ChannelSvcs/ChannelManagerController/ChannelSessionFactory.hpp>
#include <ChannelSvcs/ChannelManagerController/ChannelLoadSessionFactory.hpp>

namespace AdServer
{
namespace ChannelSvcs
{
  /** ChannelServerSessionFactory */
  class ChannelServerSessionFactory
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    static void init(
        const CORBACommons::CorbaClientAdapter& corba_client_adapter,
        ChannelServerSessionFactoryImpl_var* server_session_factory,
        ChannelLoadSessionFactoryImpl_var* load_session_factory,
        Generics::ActiveObjectCallback* callback,
        Logging::Logger* stat_logger,
        unsigned long count_match_threads = 12,
        unsigned long count_update_threads = 40,
        unsigned long check_period = 30)
        throw(eh::Exception);
  };

} /* ChannelSvcs */
} /* AdServer */

#endif
