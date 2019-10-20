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

#ifndef CHANNELSERVERSESSIONPOOL_HPP
#define CHANNELSERVERSESSIONPOOL_HPP

#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ObjectPool.hpp>

#include <ChannelSvcs/ChannelManagerController/ChannelManagerController.hpp>
#include <ChannelSvcs/ChannelManagerController/ChannelServerSessionFactory.hpp>

namespace FrontendCommons
{
  class ChannelServerSessionPool
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    ChannelServerSessionPool(
      const CORBACommons::CorbaObjectRefList& controller_refs,
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      Generics::ActiveObjectCallback* callback)
      throw(Exception);

    void match(
      const AdServer::ChannelSvcs::ChannelServerBase::MatchQuery& query,
      AdServer::ChannelSvcs::ChannelServerBase::MatchResult_out result)
      throw(Exception);

    AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq*
    get_ccg_traits(
      const AdServer::ChannelSvcs::ChannelIdSeq& ids)
      throw(Exception);

  private:
    struct ChannelServerSessionPoolConfig:
      public CORBACommons::ObjectPoolRefConfiguration
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      ChannelServerSessionPoolConfig(
        const CORBACommons::CorbaClientAdapter* corba_client_adapter) throw ()
        : ObjectPoolRefConfiguration(corba_client_adapter),
          resolver(corba_client_adapter)
      {}

      struct Resolver
      {
        Resolver(
          const CORBACommons::CorbaClientAdapter* corba_client_adapter)
          throw();

        template<typename PoolType>
        PoolType*
        resolve(const ObjectRef& ref)
          throw(Exception);

      private:
        CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
      };

      Resolver resolver;
    };

    typedef CORBACommons::ObjectPool<
      AdServer::ChannelSvcs::ChannelServerBase,
      ChannelServerSessionPoolConfig>
      ChannelServerSessionPoolImpl;

    typedef ChannelServerSessionPoolImpl::ObjectHandlerType
      ChannelServerSessionHandler;

    typedef std::unique_ptr<ChannelServerSessionPoolImpl>
      ChannelServerSessionPoolImplPtr;

  private:
    ChannelServerSessionFactoryImpl_var channel_session_factory_;
    ChannelServerSessionPoolImplPtr pool_;
  };
}

#endif /*CHANNELSERVERSESSIONPOOL_HPP*/
