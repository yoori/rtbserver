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

#ifndef _USER_INFO_SVCS_USER_INFO_EXCHANGER_MAIN_HPP_
#define _USER_INFO_SVCS_USER_INFO_EXCHANGER_MAIN_HPP_

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/UserInfoSvcs/UserInfoExchangerConfig.hpp>
#include <UserInfoSvcs/UserInfoExchanger/UserInfoExchanger.hpp>

#include "UserInfoExchangerImpl.hpp"


class UserInfoExchangerApp_
  : public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  UserInfoExchangerApp_() throw (eh::Exception);

  void main(int& argc, char** argv) throw();

protected:
  /** CorbaCommons::ProcessControlImpl methods */
  virtual void shutdown(CORBA::Boolean wait_for_completion)
    throw(CORBA::SystemException);

  virtual CORBACommons::IProcessControl::ALIVE_STATUS
  is_alive() throw(CORBA::SystemException);

private:
  typedef
    AdServer::UserInfoSvcs::
    UserInfoExchangerImpl::UserInfoExchangerConfig
    ConfigType;

  virtual ~UserInfoExchangerApp_() throw(){};

  const ConfigType& config() const throw();

private:

  typedef std::unique_ptr<ConfigType> ConfigPtr;

  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::UserInfoSvcs::UserInfoExchangerImpl_var
    user_info_exchanger_impl_;

  ConfigPtr configuration_;

  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<UserInfoExchangerApp_>
  UserInfoExchangerApp_var;

typedef Generics::Singleton<UserInfoExchangerApp_, UserInfoExchangerApp_var>
  UserInfoExchangerApp;


/** Inlines */

inline
const UserInfoExchangerApp_::ConfigType&
UserInfoExchangerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*_USER_INFO_SVCS_USER_INFO_EXCHANGER_MAIN_HPP_*/
