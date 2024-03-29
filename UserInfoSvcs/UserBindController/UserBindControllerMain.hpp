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

#ifndef USERINFOSVCS_USERBINDCONTROLLERMAIN_HPP_
#define USERINFOSVCS_USERBINDCONTROLLERMAIN_HPP_

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/UserInfoSvcs/UserBindControllerConfig.hpp>

#include <UserInfoSvcs/UserBindController/UserBindController.hpp>

#include "UserBindControllerImpl.hpp"

class UserBindControllerApp_
  : public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  UserBindControllerApp_() throw (eh::Exception);

  void main(int& argc, char** argv) throw();

protected:
  typedef AdServer::UserInfoSvcs::
    UserBindControllerImpl::UserBindControllerConfig
    ConfigType;

  typedef std::unique_ptr<ConfigType> ConfigPtr;

  const ConfigType& config() const throw();

protected:
  virtual
  ~UserBindControllerApp_() throw(){};

  // IProcessControl
  virtual void shutdown(CORBA::Boolean wait_for_completion)
    throw(CORBA::SystemException);

  virtual CORBACommons::IProcessControl::ALIVE_STATUS
  is_alive() throw(CORBA::SystemException);

  virtual Logging::Logger*
  logger() const throw ();

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::UserInfoSvcs::UserBindControllerImpl_var
    user_bind_controller_impl_;

  ConfigPtr configuration_;
  Logging::Logger_var logger_;

  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<UserBindControllerApp_>
  UserBindControllerApp_var;

typedef Generics::Singleton<
  UserBindControllerApp_, UserBindControllerApp_var>
  UserBindControllerApp;

// Inlines
inline
Logging::Logger*
UserBindControllerApp_::logger() const throw()
{
  return logger_;
}

inline
const UserBindControllerApp_::ConfigType&
UserBindControllerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*USERINFOSVCS_USERBINDCONTROLLERMAIN_HPP_*/
