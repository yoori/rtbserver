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

#ifndef USERINFOSVCS_USERBINDSERVERMAIN_HPP
#define USERINFOSVCS_USERBINDSERVERMAIN_HPP

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/UserInfoSvcs/UserBindServerConfig.hpp>

#include "UserBindServerImpl.hpp"

class UserBindServerApp_
  : public AdServer::Commons::ProcessControlVarsLoggerImpl,
    public virtual Generics::CompositeActiveObject
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  UserBindServerApp_() throw (eh::Exception);

  void main(int& argc, char** argv) throw();

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
  virtual ~UserBindServerApp_() throw(){};

  const AdServer::UserInfoSvcs::UserBindServerImpl::UserBindServerConfig&
    config() const throw();
  
  typedef std::unique_ptr<
    AdServer::UserInfoSvcs::UserBindServerImpl::UserBindServerConfig>
    ConfigPtr;

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::UserInfoSvcs::UserBindServerImpl_var
    user_bind_server_impl_;
  
  ConfigPtr configuration_;
  
  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<UserBindServerApp_>
  UserBindServerApp_var;

typedef Generics::Singleton<UserBindServerApp_, UserBindServerApp_var>
  UserBindServerApp;

// Inlines
inline
const AdServer::UserInfoSvcs::UserBindServerImpl::UserBindServerConfig&
UserBindServerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*USERINFOSVCS_USERBINDSERVERMAIN_HPP*/
