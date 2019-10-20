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

#ifndef AD_SERVER_CHANNEL_SERVER_CONTROL_IMPL_HPP_
#define AD_SERVER_CHANNEL_SERVER_CONTROL_IMPL_HPP_


#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <eh/Exception.hpp>
#include <Logger/Logger.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/Time.hpp>
#include <ChannelSvcs/ChannelServer/ChannelServer_s.hpp>
#include "ChannelServerImpl.hpp"

namespace AdServer
{
namespace ChannelSvcs
{
  /**
   * Implementation of common part ChannelServer
   */

  class ChannelServerControlImpl:
    public virtual CORBACommons::ReferenceCounting::ServantImpl
      <POA_AdServer::ChannelSvcs::ChannelServerControl>
  {

  public:

    ChannelServerControlImpl(ChannelServerCustomImpl* custom) throw ();

  protected:
    virtual ~ChannelServerControlImpl() throw();
  public:

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/set_sources:1.0
    //
    virtual void set_sources(const ::AdServer::ChannelSvcs::
      ChannelServerControl::DBSourceInfo& db_info,
      const ::AdServer::ChannelSvcs::ChunkKeySeq& sources)
      throw(
      AdServer::ChannelSvcs::ImplementationException);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/set_proxy_sources:1.0
    //
    virtual void set_proxy_sources(const ::AdServer::ChannelSvcs::
      ChannelServerControl::ProxySourceInfo& poxy_info,
      const ::AdServer::ChannelSvcs::ChunkKeySeq& sources)
      throw(
          AdServer::ChannelSvcs::ImplementationException);

    //
    // IDL:AdServer/ChannelSvcs/ChannelServerControl/get_queries_counter:1.0
    //
    virtual ::CORBA::ULong check_configuration() throw();

  protected:

    typedef Sync::PosixRWLock Mutex_;
    typedef Sync::PosixRGuard ReadGuard_;
    typedef Sync::PosixWGuard WriteGuard_;

    mutable Mutex_ lock_;
  private:

    ChannelServerCustomImpl_var custom_impl_;
  };

  typedef ReferenceCounting::SmartPtr<ChannelServerControlImpl>
    ChannelServerControlImpl_var;

} /* ChannelSvcs */
} /* AdServer */

#endif /*AD_SERVER_CHANNEL_SERVER_CONTROL_IMPL_HPP_*/

