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

#ifndef CAMPAIGNSVCS_BILLINGSERVERMAIN_HPP
#define CAMPAIGNSVCS_BILLINGSERVERMAIN_HPP

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>
#include <Generics/Singleton.hpp>
#include <Sync/SyncPolicy.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>

#include <xsd/CampaignSvcs/BillingServerConfig.hpp>

#include "BillingServerImpl.hpp"

class BillingServerApp_
  : public AdServer::Commons::ProcessControlVarsLoggerImpl,
    public virtual Generics::CompositeActiveObject
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

public:
  BillingServerApp_() throw (eh::Exception);

  void
  main(int argc, char** argv) throw();

protected:
  typedef Sync::Policy::PosixThread ShutdownSyncPolicy;

  typedef std::unique_ptr<
    AdServer::CampaignSvcs::BillingServerImpl::BillingServerConfig>
    ConfigPtr;

protected:
  virtual ~BillingServerApp_() throw() {};

  // ProcessControl interface
  virtual void
  shutdown(CORBA::Boolean wait_for_completion)
    throw(CORBA::SystemException);

  virtual CORBACommons::IProcessControl::ALIVE_STATUS
  is_alive() throw(CORBA::SystemException);

  const AdServer::CampaignSvcs::BillingServerImpl::BillingServerConfig&
  config() const throw();

private:
  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;

  AdServer::CampaignSvcs::BillingServerImpl_var billing_server_impl_;

  ConfigPtr configuration_;

  ShutdownSyncPolicy::Mutex shutdown_lock_;
};

typedef ReferenceCounting::SmartPtr<BillingServerApp_>
  BillingServerApp_var;

typedef Generics::Singleton<BillingServerApp_, BillingServerApp_var>
  BillingServerApp;

// Inlines
inline
const AdServer::CampaignSvcs::BillingServerImpl::BillingServerConfig&
BillingServerApp_::config() const throw()
{
  return *configuration_.get();
}

#endif /*CAMPAIGNSVCS_BILLINGSERVERMAIN_HPP*/
