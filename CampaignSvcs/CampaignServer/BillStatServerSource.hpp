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

#ifndef CAMPAIGNSERVER_BILLSTATSERVERSOURCE_HPP
#define CAMPAIGNSERVER_BILLSTATSERVERSOURCE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <Commons/CorbaObject.hpp>
#include "CampaignServerPool.hpp"
#include "BillStatSource.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
  class BillStatServerSource:
    public BillStatSource,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    BillStatServerSource(
      Logging::Logger* logger,
      unsigned long server_id,
      const CORBACommons::CorbaObjectRefList& stat_providers)
      throw();

    virtual Stat_var
    update(
      Stat* stat,
      const Generics::Time& now)
      throw(BillStatSource::Exception);

  protected:
    virtual
    ~BillStatServerSource() throw() = default;

    Stat_var
    convert_update_(
      const AdServer::CampaignSvcs::BillStatInfo& bill_stat_info,
      const Generics::Time& now)
      throw(Exception);

    void
    convert_amount_distribution_(
      Stat::AmountDistribution& amount_distribution,
      const AmountDistributionInfo& amount_distribution_info)
      throw();

  private:
    Logging::Logger_var logger_;
    const unsigned long server_id_;
    CampaignServerPoolPtr campaign_servers_;
  };

  typedef ReferenceCounting::QualPtr<BillStatServerSource>
    BillStatServerSource_var;
}
}

#endif /*CAMPAIGNSERVER_BILLSTATSERVERSOURCE_HPP*/
