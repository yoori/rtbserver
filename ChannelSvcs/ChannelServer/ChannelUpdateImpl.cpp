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

#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <eh/Exception.hpp>
#include <Logger/Logger.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ServantImpl.hpp>
#include <ChannelSvcs/ChannelServer/ChannelServer_s.hpp>
#include "ChannelServerImpl.hpp"
#include "ChannelUpdateImpl.hpp"

namespace AdServer
{
namespace ChannelSvcs
{

  ChannelUpdateImpl::ChannelUpdateImpl(ChannelServerCustomImpl* server)
    throw(eh::Exception)
    : server_(ReferenceCounting::add_ref(server))
  {
  }

  ChannelUpdateImpl::~ChannelUpdateImpl() throw()
  {
  }

  //
  // IDL:AdServer/ChannelSvcs/ChannelCurrent/check:1.0
  //
  void ChannelUpdateImpl::check(
    const ::AdServer::ChannelSvcs::ChannelCurrent::CheckQuery& query,
    ::AdServer::ChannelSvcs::ChannelCurrent::CheckData_out data)
    throw(AdServer::ChannelSvcs::ImplementationException,
          AdServer::ChannelSvcs::NotConfigured)
  {
    server_->check(query, data);
  }

  //
  // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_channels:1.0
  //
  void ChannelUpdateImpl::update_triggers(
    const ::AdServer::ChannelSvcs::ChannelIdSeq& ids,
    ::AdServer::ChannelSvcs::ChannelCurrent::UpdateData_out result)
    throw(AdServer::ChannelSvcs::ImplementationException,
          AdServer::ChannelSvcs::NotConfigured)
  {
    server_->update_triggers(ids, result);
  }

  //
  // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_all_ccg:1.0
  //
  void ChannelUpdateImpl::update_all_ccg(
    const AdServer::ChannelSvcs::ChannelCurrent::CCGQuery& query,
    AdServer::ChannelSvcs::ChannelCurrent::PosCCGResult_out result)
    throw(AdServer::ChannelSvcs::ImplementationException,
          AdServer::ChannelSvcs::NotConfigured)
  {
    server_->update_all_ccg(query, result);
  }

  //
  // IDL:AdServer/ChannelSvcs/ChannelProxy/get_count_chunks:1.0
  //
  ::CORBA::ULong ChannelUpdateImpl::get_count_chunks()
    throw(AdServer::ChannelSvcs::ImplementationException)
  {
    return server_->get_count_chunks();
  }

}
}

