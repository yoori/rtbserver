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

#ifndef _USER_INFO_SVCS_USER_INFO_MANAGER_CONTROLLER_MAIN_HPP_
#define _USER_INFO_SVCS_USER_INFO_MANAGER_CONTROLLER_MAIN_HPP_

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/UserInfoSvcs/UserInfoManagerControllerConfig.hpp>

#include <UserInfoSvcs/UserInfoManagerController/UserInfoManagerController.hpp>

#include "UserInfoManagerControllerImpl.hpp"

class UserInfoManagerControllerApp_
  : public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  UserInfoManagerControllerApp_() throw (eh::Exception);

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
  virtual ~UserInfoManagerControllerApp_() throw(){};

  virtual Logging::Logger*
  logger() const throw ();

  typedef
    AdServer::UserInfoSvcs::
    UserInfoManagerControllerImpl::UserInfoManagerControllerConfig
    ConfigType;

  typedef std::unique_ptr<ConfigType> ConfigPtr;

protected:
  const ConfigType& config() const throw();

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::UserInfoSvcs::UserInfoManagerControllerImpl_var
    user_info_manager_controller_impl_;

  ConfigPtr configuration_;
  Logging::Logger_var logger_;

  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<UserInfoManagerControllerApp_>
  UserInfoManagerControllerApp_var;

typedef Generics::Singleton<
  UserInfoManagerControllerApp_, UserInfoManagerControllerApp_var>
  UserInfoManagerControllerApp;


//////////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////////

inline
Logging::Logger*
UserInfoManagerControllerApp_::logger() const throw()
{
  return logger_;
}

inline
const UserInfoManagerControllerApp_::ConfigType&
UserInfoManagerControllerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*_USER_INFO_SVCS_USER_INFO_MANAGER_CONTROLLER_MAIN_HPP_*/
