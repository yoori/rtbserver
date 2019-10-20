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


#ifndef AD_CHANNEL_SVCS_DICTIONARY_PROVIDER_HPP_
#define AD_CHANNEL_SVCS_DICTIONARY_PROVIDER_HPP_

#include <eh/Exception.hpp>

#include <Generics/Singleton.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/ChannelSvcs/DictionaryProviderConfig.hpp>
#include "DictionaryProviderImpl.hpp"


class DictionaryProviderApp_ :
  public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  DictionaryProviderApp_() throw (eh::Exception);
  virtual ~DictionaryProviderApp_() throw(){};

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

  typedef std::unique_ptr<xsd::AdServer::Configuration::
    DictionaryProviderConfigType> ConfigPtr;

  void load_config_(const char* name) throw(Exception);
  void init_logger_() throw(Exception);
  void init_corba_() throw(Exception, CORBA::SystemException);

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;
  AdServer::ChannelSvcs::DictionaryProviderImpl_var server_impl_;

  ConfigPtr configuration_;

  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<DictionaryProviderApp_> DictionaryProviderApp_var;

typedef Generics::Singleton<DictionaryProviderApp_, DictionaryProviderApp_var>
  DictionaryProviderApp;

#endif /*AD_CHANNEL_SVCS_DICTIONARY_PROVIDER_HPP_*/
