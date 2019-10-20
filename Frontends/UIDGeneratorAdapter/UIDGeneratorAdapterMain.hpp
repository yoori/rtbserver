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

#ifndef UIDGENERATORADAPTERMAIN_HPP_
#define UIDGENERATORADAPTERMAIN_HPP_

#include <eh/Exception.hpp>
#include <Generics/Singleton.hpp>
#include <Logger/ActiveObjectCallback.hpp>
#include <Sync/SyncPolicy.hpp>

#include <Commons/CorbaConfig.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <Frontends/UIDGeneratorAdapterConfig.hpp>

#include "UIDGeneratorAdapterImpl.hpp"

class UIDGeneratorAdapterApp_:
  public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  virtual void
  shutdown(CORBA::Boolean wait_for_completion)
    throw(CORBA::SystemException);

  virtual CORBACommons::IProcessControl::ALIVE_STATUS
  is_alive() throw(CORBA::SystemException);

  void
  main(int& argc, char** argv) throw();

protected:
  typedef Sync::Policy::PosixThread ShutdownSyncPolicy;

  typedef AdServer::Frontends::UIDGeneratorAdapterImpl::Config
    Config;

  typedef std::unique_ptr<Config> ConfigPtr;

protected:
  virtual
  ~UIDGeneratorAdapterApp_() throw();

  void
  init_corba_() throw(Exception);

  void
  read_config_(
    const char *filename,
    const char* argv0)
    throw(Exception, eh::Exception);

protected:
  ConfigPtr config_;
  CORBACommons::CorbaConfig corba_config_;
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  AdServer::Frontends::UIDGeneratorAdapterImpl_var uid_generator_adapter_impl_;

  ShutdownSyncPolicy::Mutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<UIDGeneratorAdapterApp_>
  UIDGeneratorAdapterApp_var;

typedef Generics::Singleton<
  UIDGeneratorAdapterApp_, UIDGeneratorAdapterApp_var>
  UIDGeneratorAdapterApp;

#endif /*UIDGENERATORADAPTERMAIN_HPP_*/
