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

#ifndef CAMPAIGNSVCS_BILLINGSERVERIMPL_HPP
#define CAMPAIGNSVCS_BILLINGSERVERIMPL_HPP

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/PtrHolder.hpp>

#include <Logger/Logger.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/Time.hpp>

#include <CORBACommons/ServantImpl.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <Commons/CorbaConfig.hpp>
#include <Commons/AccessActiveObject.hpp>
#include <ReferenceCounting/PtrHolder.hpp>

#include <xsd/AdServerCommons/AdServerCommons.hpp>
#include <xsd/CampaignSvcs/BillingServerConfig.hpp>

#include <CampaignSvcs/CampaignServer/CampaignServerPool.hpp>
#include <CampaignSvcs/CampaignServer/BillStatSource.hpp>

#include <CampaignSvcs/BillingServer/BillingServer_s.hpp>

#include "BillingContainer.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
  /**
   * Implementation of BillingServer.
   */
  class BillingServerImpl:
    public virtual CORBACommons::ReferenceCounting::ServantImpl<
      POA_AdServer::CampaignSvcs::BillingServer>,
    public virtual Generics::CompositeActiveObject
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    typedef xsd::AdServer::Configuration::BillingServerConfigType
      BillingServerConfig;

    BillingServerImpl(
      Generics::ActiveObjectCallback* callback,
      Logging::Logger* logger,
      const BillingServerConfig& billing_server_config)
      throw(Exception);

    virtual AdServer::CampaignSvcs::BillingServer::BidResultInfo*
    check_available_bid(
      const AdServer::CampaignSvcs::BillingServer::CheckBidInfo& request_info)
      throw(AdServer::CampaignSvcs::BillingServer::NotReady,
        AdServer::CampaignSvcs::BillingServer::ImplementationException);

    virtual AdServer::CampaignSvcs::BillingServer::BidResultInfo*
    confirm_bid(
      AdServer::CampaignSvcs::BillingServer::ConfirmBidInfo& request_info)
      throw(AdServer::CampaignSvcs::BillingServer::NotReady,
        AdServer::CampaignSvcs::BillingServer::ImplementationException);

    virtual bool
    reserve_bid(
      const AdServer::CampaignSvcs::BillingServer::ReserveBidInfo& request_info)
      throw(AdServer::CampaignSvcs::BillingServer::NotReady,
        AdServer::CampaignSvcs::BillingServer::ImplementationException);

    virtual void
    add_amount(
      AdServer::CampaignSvcs::BillingServer::ConfirmBidRefSeq_out remainder_request_seq,
      const AdServer::CampaignSvcs::BillingServer::ConfirmBidSeq& request_seq)
      throw(AdServer::CampaignSvcs::BillingServer::NotReady,
        AdServer::CampaignSvcs::BillingServer::ImplementationException);

    virtual void
    wait_object()
      throw (Generics::ActiveObject::Exception, eh::Exception);

    Logging::Logger*
    logger() throw();

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;

    typedef AdServer::Commons::AccessActiveObject<
      BillingProcessor_var>
      BillingProcessorHolder;

    typedef ReferenceCounting::SmartPtr<BillingProcessorHolder>
      BillingProcessorHolder_var;

  protected:
    virtual
    ~BillingServerImpl() throw() {};

    BillingProcessorHolder::Accessor
    get_accessor_()
      throw(AdServer::CampaignSvcs::BillingServer::NotReady);

    void
    apply_delivery_limitation_config_update_(
      BillingContainer::Config& res_config,
      const AdServer::CampaignSvcs::CampaignServer::
        DeliveryLimitConfigInfo& config)
      throw(Exception);

    // tasks
    Generics::Time
    load_() throw();

    Generics::Time
    update_config_() throw();

    Generics::Time
    update_stat_() throw();

    void
    clear_expired_reservation_()
      throw();

    void
    dump_() throw();

  private:
    Generics::ActiveObjectCallback_var callback_;
    Logging::Logger_var logger_;
    Generics::Planner_var scheduler_;
    Generics::TaskRunner_var task_runner_;

    const BillingServerConfig config_;

    CampaignServerPoolPtr campaign_servers_;
    BillStatSource_var bill_stat_source_;
    BillingProcessorHolder_var billing_processor_;
    ReferenceCounting::PtrHolder<BillingContainer_var> billing_container_;
  };

  typedef ReferenceCounting::SmartPtr<BillingServerImpl>
    BillingServerImpl_var;

} /* CampaignSvcs */
} /* AdServer */

namespace AdServer
{
namespace CampaignSvcs
{
  inline
  Logging::Logger*
  BillingServerImpl::logger() throw()
  {
    return logger_;
  }
}
}

#endif /*CAMPAIGNSVCS_BILLINGSERVERIMPL_HPP*/
