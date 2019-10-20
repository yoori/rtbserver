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

#ifndef CAMPAIGNSVCS_CAMPAIGNSERVER_CAMPAIGNSERVERIMPL_V331_HPP
#define CAMPAIGNSVCS_CAMPAIGNSERVER_CAMPAIGNSERVERIMPL_V331_HPP

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Generics/ActiveObject.hpp>
#include <Logger/Logger.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ServantImpl.hpp>

/* AdServer::CampaignSvcs_v330 */
#include <CampaignSvcs/CampaignServer/Compatibility/CampaignCommons_v331.hpp>
#include <CampaignSvcs/CampaignServer/Compatibility/CampaignServer_v331.hpp>

#include <CampaignSvcs/CampaignServer/Compatibility/CampaignCommons_v331_s.hpp>
#include <CampaignSvcs/CampaignServer/Compatibility/CampaignServer_v331_s.hpp>

/* AdServer::CampaignSvcs_v340 */
#include <CampaignSvcs/CampaignServer/Compatibility/CampaignCommons_v340.hpp>
#include <CampaignSvcs/CampaignServer/Compatibility/CampaignServer_v340.hpp>

#include <CampaignSvcs/CampaignServer/Compatibility/CampaignCommons_v340_s.hpp>
#include <CampaignSvcs/CampaignServer/Compatibility/CampaignServer_v340_s.hpp>

namespace AdServer
{
  namespace CampaignSvcs
  {
    class CampaignServerImpl_v331:
      public virtual CORBACommons::ReferenceCounting::
        ServantImpl<POA_AdServer::CampaignSvcs_v330::CampaignServer>
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      DECLARE_EXCEPTION(InvalidArgument, Exception);

    public:
      /** */
      CampaignServerImpl_v331(
        Logging::Logger* logger,
        POA_AdServer::CampaignSvcs_v340::CampaignServer* campaign_server)
        throw(InvalidArgument, Exception, eh::Exception);

    public:
      typedef Generics::SimpleDecimal<uint64_t, 18, 8> RevenueDecimal_v300;

      virtual CORBA::Boolean
      need_config(const AdServer::CampaignSvcs_v330::TimestampInfo& master_stamp)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException);

      virtual AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo*
      get_config(
        const AdServer::CampaignSvcs_v330::CampaignGetConfigSettings& settings)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
              AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual AdServer::CampaignSvcs_v330::EcpmSeq*
      get_ecpms(
        const AdServer::CampaignSvcs_v330::TimestampInfo& request_timestamp)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException);

      virtual AdServer::CampaignSvcs_v330::BriefSimpleChannelAnswer*
      brief_simple_channels(
        const AdServer::CampaignSvcs_v330::CampaignServer::
          GetSimpleChannelsInfo& settings)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
          AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual AdServer::CampaignSvcs_v330::SimpleChannelAnswer*
      simple_channels(
        const AdServer::CampaignSvcs_v330::CampaignServer::GetSimpleChannelsInfo& settings)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
              AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::ExpressionChannelsInfo*
      get_expression_channels(
        const AdServer::CampaignSvcs_v330::CampaignServer::GetExpressionChannelsInfo&)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
          AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::DiscoverSourceInfo*
      get_discover_channels(CORBA::ULong, CORBA::ULong)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
              AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::CampaignServer::PassbackInfo*
      get_tag_passback(CORBA::ULong, const char*)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
              AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::FraudConditionConfig*
      fraud_conditions()
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
          AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::DetectorsConfig*
      detectors(const AdServer::CampaignSvcs_v330::TimestampInfo& request_timestamp)
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
          AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::FreqCapConfigInfo*
      freq_caps()
        throw(AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
          AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::StatInfo*
      get_stat() throw(
        AdServer::CampaignSvcs_v330::CampaignServer::NotSupport,
        AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      AdServer::CampaignSvcs_v330::ColocationFlagsSeq*
      get_colocation_flags()
        throw (AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException,
          AdServer::CampaignSvcs_v330::CampaignServer::NotReady);

      virtual
      void update_stat() throw(
        AdServer::CampaignSvcs_v330::CampaignServer::NotSupport,
        AdServer::CampaignSvcs_v330::CampaignServer::ImplementationException);

    protected:
      virtual
      ~CampaignServerImpl_v331() throw ();

      typedef PortableServer::Servant_var<
        POA_AdServer::CampaignSvcs_v340::CampaignServer>
        CampaignServer_var;

    protected:
      void convert_config_(
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const Generics::Time& request_timestamp)
        throw(Exception, eh::Exception);

      void convert_get_config_settings_(
        const AdServer::CampaignSvcs_v330::CampaignGetConfigSettings& settings,
        AdServer::CampaignSvcs_v340::CampaignGetConfigSettings& result_settings)
        throw(Exception, eh::Exception);

      void
      convert_sizes_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      void convert_accounts_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      void convert_campaigns_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        const
        throw();

      void convert_creative_options_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_expression_channels_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_ecpm_seq_(
        AdServer::CampaignSvcs_v330::EcpmSeq& result_ecpms,
        const AdServer::CampaignSvcs_v340::EcpmSeq& ecpms,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_ecpms_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_sites_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_tags_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_frequency_caps_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_simple_channels_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_geo_channels_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_behav_params_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_colocations_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_creative_templates_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_currencies_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_campaign_keywords_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_creative_categories_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_adv_actions_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_category_channels_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_margin_rules_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_fraud_conditions_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_search_engines_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_web_browsers_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_platforms_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_string_dictionaries_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

      static void convert_app_formats_(
        AdServer::CampaignSvcs_v330::CampaignConfigUpdateInfo& result_update_info,
        const AdServer::CampaignSvcs_v340::CampaignConfigUpdateInfo& update_info,
        const Generics::Time& request_timestamp)
        throw();

    private:
      Logging::Logger_var logger_;
      CampaignServer_var campaign_server_;
    };

    typedef ReferenceCounting::SmartPtr<CampaignServerImpl_v331>
      CampaignServerImpl_v331_var;
  }
}

#endif // CAMPAIGNSVCS_CAMPAIGNSERVER_CAMPAIGNSERVERIMPL_V331_HPP
