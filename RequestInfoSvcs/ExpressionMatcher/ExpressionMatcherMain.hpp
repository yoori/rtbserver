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

#ifndef _CAMPAIGN_SVCS_EXPRESSION_MATCHER_MAIN_HPP_
#define _CAMPAIGN_SVCS_EXPRESSION_MATCHER_MAIN_HPP_

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>
#include <SNMPAgent/SNMPAgentX.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include "ExpressionMatcherImpl.hpp"
#include "ExpressionMatcherStats.hpp"

class ExpressionMatcherApp_ :
  public AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

  typedef
    AdServer::RequestInfoSvcs::ExpressionMatcherImpl::ExpressionMatcherConfig
    Configuration;

public:
  ExpressionMatcherApp_() throw (eh::Exception);

  void main(int& argc, char** argv) throw();

protected:
  /* ProcessControl interface */
  virtual void shutdown(CORBA::Boolean wait_for_completion)
    throw(CORBA::SystemException);

  virtual CORBACommons::IProcessControl::ALIVE_STATUS
  is_alive() throw(CORBA::SystemException);

private:
  virtual ~ExpressionMatcherApp_() throw(){};

  const Configuration& config() const throw();

  typedef std::unique_ptr<Configuration> ConfigPtr;

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::RequestInfoSvcs::ExpressionMatcherImpl_var
    expression_matcher_impl_;

  ConfigPtr configuration_;
  SNMPAgentX::SNMPStatsImpl_var snmp_stat_provider_;

  typedef Sync::Policy::PosixThread SyncPolicy;
  typedef SyncPolicy::Mutex ShutdownMutex;
  typedef SyncPolicy::WriteGuard ShutdownGuard;

  ShutdownMutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<ExpressionMatcherApp_>
  ExpressionMatcherApp_var;

typedef Generics::Singleton<ExpressionMatcherApp_, ExpressionMatcherApp_var>
  ExpressionMatcherApp;


/*
 * Inlines
 */

inline
const ExpressionMatcherApp_::Configuration&
ExpressionMatcherApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*_CAMPAIGN_SVCS_EXPRESSION_MATCHER_MAIN_HPP_*/
