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

#ifndef _AD_SERVER_CHANNEL_SEARCH_SVCS_CHANNEL_SEARCH_SERVICE_MAIN_HPP_
#define _AD_SERVER_CHANNEL_SEARCH_SVCS_CHANNEL_SEARCH_SERVICE_MAIN_HPP_


#include <eh/Exception.hpp>
#include <Generics/Singleton.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>
#include <xsd/ChannelSearchSvcs/ChannelSearchServiceConfig.hpp>
#include "ChannelSearchServiceImpl.hpp"

class ChannelSearchServiceApp_ :
  public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  ChannelSearchServiceApp_() throw(eh::Exception);

  void main(int argc, char **argv) throw();

protected:
  //
  // IDL:CORBACommons/IProcessControl/shutdown:1.0
  //
  virtual void shutdown(CORBA::Boolean wait_for_completion)
    throw(CORBA::SystemException);

  //
  // IDL:CORBACommons/IProcessControl/is_alive:1.0
  //
  virtual CORBACommons::IProcessControl::ALIVE_STATUS
  is_alive() throw(CORBA::SystemException);

private:
  virtual ~ChannelSearchServiceApp_() throw() {}
  const xsd::AdServer::Configuration::ChannelSearchServiceConfigType&
    config() const throw();

  typedef std::unique_ptr<
    xsd::AdServer::Configuration::ChannelSearchServiceConfigType>
    ConfigPtr;

private:
  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ConfigPtr configuration_;
  ShutdownMutex shutdown_lock_;
  CORBACommons::CorbaConfig corba_config_;
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  AdServer::ChannelSearchSvcs::ChannelSearchServiceImpl_var service_impl_;
};

typedef ReferenceCounting::SmartPtr<ChannelSearchServiceApp_>
  ChannelSearchServiceApp_var;

typedef Generics::Singleton<ChannelSearchServiceApp_, ChannelSearchServiceApp_var>
  ChannelSearchServiceApp;

inline
const xsd::AdServer::Configuration::ChannelSearchServiceConfigType&
ChannelSearchServiceApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /* _AD_SERVER_CHANNEL_SEARCH_SVCS_CHANNEL_SEARCH_SERVICE_MAIN_HPP_ */

