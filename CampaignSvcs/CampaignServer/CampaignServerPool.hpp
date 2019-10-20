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

#ifndef CAMPAIGNSVCS_CAMPAIGNSERVER_CAMPAIGNSERVERPOOL_HPP_
#define CAMPAIGNSVCS_CAMPAIGNSERVER_CAMPAIGNSERVERPOOL_HPP_

#include <memory>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ObjectPool.hpp>

#include <CampaignSvcs/CampaignServer/CampaignServer.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  class CampaignServerPool: public CORBACommons::ObjectPool<
    AdServer::CampaignSvcs::CampaignServer,
    CORBACommons::ObjectPoolRefConfiguration>
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    CampaignServerPool(
      const CORBACommons::CorbaObjectRefList& campaign_server_refs,
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      CORBACommons::ChoosePolicyType::POLICY_TYPE policy =
        CORBACommons::ChoosePolicyType::PT_PERSISTENT,
      const Generics::Time& bad_period = Generics::Time(10))
      throw(Exception);

  protected:
    static CORBACommons::ObjectPoolRefConfiguration
    generate_config_(
      const CORBACommons::CorbaObjectRefList& campaign_server_refs,
      const CORBACommons::CorbaClientAdapter* corba_client_adapter,
      const Generics::Time& bad_period)
      throw();
  };

  typedef std::unique_ptr<CampaignServerPool> CampaignServerPoolPtr;
}
}

namespace AdServer
{
namespace CampaignSvcs
{
  inline
  CampaignServerPool::CampaignServerPool(
    const CORBACommons::CorbaObjectRefList& campaign_server_refs,
    const CORBACommons::CorbaClientAdapter* corba_client_adapter,
    CORBACommons::ChoosePolicyType::POLICY_TYPE policy,
    const Generics::Time& bad_period)
    throw(Exception) 
  try
    : CORBACommons::ObjectPool<
        AdServer::CampaignSvcs::CampaignServer,
        CORBACommons::ObjectPoolRefConfiguration>(
          generate_config_(campaign_server_refs,
            corba_client_adapter,
            bad_period),
          policy)
  {}
  catch(const eh::Exception& ex)
  {
    static const char* FUN = "CampaignServerPool::CampaignServerPool()";

    Stream::Error ostr;
    ostr << FUN << ": caught eh::Exception: " << ex.what();
    throw Exception(ostr);
  }

  inline
  CORBACommons::ObjectPoolRefConfiguration
  CampaignServerPool::generate_config_(
    const CORBACommons::CorbaObjectRefList& campaign_server_refs,
    const CORBACommons::CorbaClientAdapter* corba_client_adapter,
    const Generics::Time& bad_period)
    throw()
  {
    CORBACommons::ObjectPoolRefConfiguration pool_config(corba_client_adapter);
    pool_config.timeout = bad_period;

    std::copy(
      campaign_server_refs.begin(),
      campaign_server_refs.end(),
      std::back_inserter(pool_config.iors_list));

    return pool_config;
  }
}
}

#endif /* CAMPAIGNSVCS_CAMPAIGNSERVER_CAMPAIGNSERVERPOOL_HPP_ */
