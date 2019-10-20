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

#ifndef _USER_INFO_SVCS_USER_INFO_MANAGER_MAIN_HPP_
#define _USER_INFO_SVCS_USER_INFO_MANAGER_MAIN_HPP_

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/UserInfoSvcs/UserInfoManagerConfig.hpp>

#include "UserInfoManager.hpp"
#include "UserInfoManagerImpl.hpp"
#include "UserInfoManagerControlImpl.hpp"

class UserInfoManagerApp_
  : public AdServer::Commons::ProcessControlVarsLoggerImpl,
    public virtual Generics::CompositeActiveObject
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  UserInfoManagerApp_() throw (eh::Exception);

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

  virtual bool is_ready_() throw();

  virtual char* comment()
    throw (CORBACommons::OutOfMemory);
private:
  virtual ~UserInfoManagerApp_() throw(){};

  const AdServer::UserInfoSvcs::UserInfoManagerImpl::UserInfoManagerConfig&
    config() const throw();
  
  typedef std::unique_ptr<
    AdServer::UserInfoSvcs::UserInfoManagerImpl::UserInfoManagerConfig>
    ConfigPtr;

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::UserInfoSvcs::UserInfoManagerImpl_var
    user_info_manager_impl_;
  AdServer::UserInfoSvcs::UserInfoManagerControlImpl_var
    user_info_manager_control_impl_;
  
  ConfigPtr configuration_;
  
  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<UserInfoManagerApp_>
  UserInfoManagerApp_var;

typedef Generics::Singleton<UserInfoManagerApp_, UserInfoManagerApp_var>
  UserInfoManagerApp;


//////////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////////

inline
const AdServer::UserInfoSvcs::UserInfoManagerImpl::UserInfoManagerConfig&
UserInfoManagerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*_USER_INFO_SVCS_USER_INFO_MANAGER_MAIN_HPP_*/
