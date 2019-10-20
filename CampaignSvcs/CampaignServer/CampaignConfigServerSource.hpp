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

#ifndef _CAMPAIGNCONFIGSERVERSOURCE_HPP_
#define _CAMPAIGNCONFIGSERVERSOURCE_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ObjectPool.hpp>

#include <CampaignSvcs/CampaignServer/CampaignServerPool.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>

#include "CampaignConfig.hpp"

namespace AdServer
{
  namespace CampaignSvcs
  {
    class CampaignConfigServerSource:
      public CampaignConfigSource,
      public ReferenceCounting::AtomicImpl
    {
    public:
      CampaignConfigServerSource(
        Logging::Logger* logger,
        unsigned long server_id,
        const CORBACommons::CorbaObjectRefList& campaign_server_refs,
        unsigned long colo_id,
        const char* version,
        const char* campaign_statuses,
        const String::SubString& channel_statuses,
        const char* country,
        bool only_tags)
        throw(Exception);

      virtual CampaignConfig_var update(bool* need_logging) throw(Exception);

    private:
      DECLARE_EXCEPTION(InvalidObject, eh::DescriptiveException);

    private:
      virtual
      ~CampaignConfigServerSource() throw ()
      {}

      void init_get_config_settings_(
        const CampaignConfig& config,
        CampaignGetConfigSettings& config_settings)
        throw();

      /* client get config adapters */
      void apply_config_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception, eh::Exception);

      void
      apply_app_formats_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void
      apply_sizes_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void
      apply_countries_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_adv_actions_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_creative_category_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_category_channel_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_margin_rules_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_bp_param_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_expression_channel_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_fraud_conditions_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_search_engines_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_web_browsers_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_platforms_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_creative_options_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void apply_web_operations_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

      void
      apply_block_channels_update_(
        const CampaignConfigUpdateInfo& update_info,
        CampaignConfig& new_config)
        throw(Exception);

    private:
      Logging::Logger_var logger_;

      const unsigned long colo_id_;
      const std::string version_;

      const std::string campaign_statuses_;
      const std::string channel_statuses_;
      std::string country_;
      bool only_tags_;
      unsigned long server_id_;

      CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
      CampaignServerPoolPtr campaign_servers_;

      CampaignConfig_var campaign_config_;
    };

    typedef ReferenceCounting::SmartPtr<CampaignConfigServerSource>
      CampaignConfigServerSource_var;
  }
}

#endif /*_CAMPAIGNCONFIGSERVERSOURCE_HPP_*/
