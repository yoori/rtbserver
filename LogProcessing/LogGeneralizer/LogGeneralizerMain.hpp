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

#ifndef AD_SERVER_LOG_PROCESSING_LOG_GENERALIZER_MAIN_HPP
#define AD_SERVER_LOG_PROCESSING_LOG_GENERALIZER_MAIN_HPP


#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>

#include <SNMPAgent/SNMPAgentX.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/StatsImpl.hpp>

#include <xsd/LogProcessing/LogGeneralizerConfig.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>
#include "LogGeneralizerImpl.hpp"
#include "ProcStatImpl.hpp"

class LogGeneralizerApp_:
  public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  LogGeneralizerApp_() throw (eh::Exception);

  void main(int &argc, char **argv) throw();

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
  typedef xsd::AdServer::Configuration::LogGeneralizerConfigType
    LogGeneralizerConfigType;

  typedef std::unique_ptr<LogGeneralizerConfigType> ConfigPtr;

  typedef Sync::PosixMutex ShutdownMutex;
  typedef Sync::PosixGuard ShutdownGuard;

  virtual ~LogGeneralizerApp_() throw() {}

  const LogGeneralizerConfigType& config() const throw();

  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  ConfigPtr configuration_;

  ShutdownMutex shutdown_lock_;

  AdServer::LogProcessing::LogGeneralizerImpl_var log_generalizer_impl_;
  AdServer::LogProcessing::ProcStatImpl_var proc_stat_impl_;
  CORBACommons::POA_ProcessStatsControl_var proc_stat_ctrl_;
  SNMPAgentX::SNMPStatsImpl_var snmp_stat_impl_;
};

typedef ReferenceCounting::SmartPtr<LogGeneralizerApp_>
  LogGeneralizerApp_var;

typedef Generics::Singleton<LogGeneralizerApp_, LogGeneralizerApp_var>
  LogGeneralizerApp;

//////////////////////////////////////////////////////////////////////////////
// Inlines
//////////////////////////////////////////////////////////////////////////////

inline
const LogGeneralizerApp_::LogGeneralizerConfigType&
LogGeneralizerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /* AD_SERVER_LOG_PROCESSING_LOG_GENERALIZER_MAIN_HPP */

