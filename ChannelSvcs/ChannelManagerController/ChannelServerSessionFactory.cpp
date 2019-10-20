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

#include<ChannelSvcs/ChannelServer/ChannelLoadSessionImpl.hpp>
#include"ChannelServerSessionFactory.hpp"

/**
 * ChannelServerSessionFactory
 */
namespace AdServer
{
namespace ChannelSvcs
{

void ChannelServerSessionFactory::init(
  const CORBACommons::CorbaClientAdapter& corba_client_adapter,
  ChannelServerSessionFactoryImpl_var* server_session_factory,
  ChannelLoadSessionFactoryImpl_var* load_session_factory,
  Generics::ActiveObjectCallback* callback,
  Logging::Logger* stat_logger,
  unsigned long count_match_threads,
  unsigned long count_update_threads,
  unsigned long check_period)
  throw(eh::Exception)
{
  /* register ChannelServerSessionFactoryImpl
     for creating Session valuetype */
  try
  {
    if(server_session_factory)
    {
      *(server_session_factory) = new ChannelServerSessionFactoryImpl(
          stat_logger, count_match_threads, callback, check_period);

      corba_client_adapter.register_value_factory(
        ChannelServerSessionImpl::_tao_obv_static_repository_id(),
        server_session_factory->in());
    }

    if(load_session_factory)
    {
      (*load_session_factory) = new ChannelLoadSessionFactoryImpl(
        count_update_threads, callback);

      corba_client_adapter.register_value_factory(
        ChannelLoadSessionImpl::_tao_obv_static_repository_id(),
        load_session_factory->in());
    }
  }
  catch(const eh::Exception& ex)
  {
    Stream::Error ostr;
    ostr << "ChannelServerSessionFactory::init(): "
         << "Can't init ChannelServerSessionFactory. "
         << ": " << ex.what();
    throw Exception(ostr);
  }
  catch(const CORBA::SystemException& e)
  {
    Stream::Error ostr;
    ostr << "ChannelServerSessionFactory::init(): "
         << "Can't init ChannelServerSessionFactory. "
            "Caught CORBA::SystemException. "
         << ": " << e;
    throw Exception(ostr);
  }
}

}// namespace ChannelSvcs
}// namespace AdServer
