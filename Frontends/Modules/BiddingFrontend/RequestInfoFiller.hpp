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

#ifndef BIDDINGFRONTEND_REQUESTINFOFILLER_HPP_
#define BIDDINGFRONTEND_REQUESTINFOFILLER_HPP_

#include <string>

#include <GeoIP/IPMap.hpp>
#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/Uuid.hpp>
#include <Generics/GnuHashTable.hpp>

#include <HTTP/Http.hpp>

#include <Commons/Containers.hpp>
#include <Commons/UserInfoManip.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserAgentMatcher.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>
#include <Frontends/FrontendCommons/FCGI.hpp>

#include <Frontends/CommonModule/CommonModule.hpp>

#include <CampaignSvcs/CampaignManager/CampaignManager.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

#include <Frontends/Modules/BiddingFrontend/google-bidding.pb.h>
#include "JsonParamProcessor.hpp"

namespace AdServer
{
namespace Bidding
{
  struct SourceTraits
  {
    enum NativeAdsInstantiateType
    {
      NAIT_NONE,
      NAIT_ADM,
      NAIT_ADM_NATIVE,
      NAIT_EXT
    };

    AdServer::Commons::Optional<unsigned long> default_account_id;
    AdServer::Commons::Optional<AdServer::CampaignSvcs::AdRequestType> request_type;
    AdServer::CampaignSvcs::AdInstantiateType instantiate_type;
    bool enabled_notice;
    std::string notice_url;
    AdServer::CampaignSvcs::AdInstantiateType vast_instantiate_type;
    bool vast_enabled_notice;
    bool ipw_extension;
    bool truncate_domain;
    std::string seat;
    AdServer::Commons::Optional<unsigned long> appnexus_member_id;
    bool fill_adid;
    AdServer::Commons::Optional<Generics::Time> max_bid_time;
    NativeAdsInstantiateType native_ads_instantiate_type;
    AdServer::Commons::Optional<AdServer::CampaignSvcs::NativeAdsImpressionTrackerType>
      native_ads_impression_tracker_type;
    bool skip_ext_category;
  };

  typedef std::map<std::string, SourceTraits>
    SourceMap;

  typedef std::map<unsigned long, std::string> DebugAdSlotSizeMap;

  struct GoogleAdSlotContext
  {
    GoogleAdSlotContext() :
      width(0),
      height(0),
      direct_deal_id(0),
      fixed_cpm_micros(std::numeric_limits<int64_t>::max())
    {}

    int width;
    int height;
    std::set<int64_t> billing_ids;
    int64_t direct_deal_id;
    int64_t fixed_cpm_micros;
  };

  typedef std::vector<GoogleAdSlotContext> GoogleAdSlotContextArray;

  struct RequestInfo
  {
    typedef std::vector<unsigned long> AccountIdArray;

    RequestInfo()
      : debug_ccg(0),
        publisher_site_id(0),
        random(CampaignSvcs::RANDOM_PARAM_MAX),
        flag(0),
        filter_request(false),
        skip_ccg_keywords(false),
        truncate_domain(false),
        ipw_extension(false),
        user_create_time(Generics::Time::ZERO),
        is_app(false),
        native_ads_instantiate_type(SourceTraits::NAIT_NONE),
        native_ads_impression_tracker_type(AdServer::CampaignSvcs::NAITT_IMP),
        skip_ext_category(false)
    {}

    Generics::Time current_time;
    std::string source_id;
    unsigned long debug_ccg;
    AccountIdArray publisher_account_ids;
    unsigned long publisher_site_id;
    unsigned long random;
    unsigned long flag;
    bool filter_request;
    bool skip_ccg_keywords;
    std::string search_words;
    std::string seat;
    AdServer::Commons::Optional<unsigned long> appnexus_member_id;
    bool truncate_domain;
    bool ipw_extension;
    std::string format;
    std::string default_debug_size;
    DebugAdSlotSizeMap debug_sizes;
    Generics::Time user_create_time;
    FrontendCommons::Location_var location;

    //std::string device_id;
    bool is_app;
    std::string application_id;
    std::string advertising_id; // ADVERTISING_ID
    std::string idfa;

    std::vector<std::string> external_user_ids;

    FrontendCommons::PlatformMatcher::PlatformNameSet platform_names;
    SourceTraits::NativeAdsInstantiateType native_ads_instantiate_type;
    AdServer::CampaignSvcs::NativeAdsImpressionTrackerType native_ads_impression_tracker_type;

    bool skip_ext_category;
    std::string notice_url;

    std::string bid_request_id;
    std::string bid_site_id;
    std::string bid_publisher_id;
  };

  class RequestInfoFiller: public FrontendCommons::HTTPExceptions
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    typedef std::set<std::string> ExternalUserIdSet;

    struct EncryptionKeys: public ReferenceCounting::AtomicImpl
    {
      EncryptionKeys()
        : google_encryption_key_size(0),
          google_integrity_key_size(0)
      {}

      Generics::ArrayAutoPtr<unsigned char> google_encryption_key;
      unsigned long google_encryption_key_size;
      Generics::ArrayAutoPtr<unsigned char> google_integrity_key;
      unsigned long google_integrity_key_size;

    protected:
      virtual ~EncryptionKeys() throw() {}
    };

    struct AccountTraits: public EncryptionKeys
    {
      AccountTraits()
        : display_billing_id(0),
          video_billing_id(0)
      {}

      Commons::Optional<CampaignSvcs::RevenueDecimal> max_cpm;
      unsigned long display_billing_id;
      unsigned long video_billing_id;

    protected:
      virtual ~AccountTraits() throw() {}
    };

    typedef ReferenceCounting::SmartPtr<AccountTraits> AccountTraits_var;
    typedef std::unordered_map<unsigned long, AccountTraits_var> AccountTraitsById;

  public:
    RequestInfoFiller(
      Logging::Logger* logger,
      unsigned long colo_id,
      CommonModule* common_module,
      const char* geo_ip_path,
      const char* user_agent_filter_path,
      const ExternalUserIdSet& skip_external_ids,
      bool ip_logging_enabled,
      const char* ip_salt,
      const SourceMap& sources,
      bool enable_profile_referer,
      const AccountTraitsById& account_traits)
      throw(eh::Exception);

    void
    fill(
      RequestInfo& request_info,
      const FCGI::HttpRequest& request,
      const Generics::Time& now) const
      throw();

    void
    fill_by_google_request(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      RequestInfo& request_info,
      std::string& keywords,
      GoogleAdSlotContextArray& as_slots_context,
      const Google::BidRequest& bid_request) const
      throw(InvalidParamException, Exception);

    // OpenRTB, Yandex
    void
    fill_by_openrtb_request(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      RequestInfo& request_info,
      std::string& keywords,
      JsonProcessingContext& context,
      const char* bid_request) const
      throw(InvalidParamException, Exception);

    void
    fill_by_appnexus_request(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      RequestInfo& request_info,
      std::string& keywords,
      JsonProcessingContext& context,
      const char* bid_request) const
      throw(InvalidParamException, Exception);

    bool
    fill_adid(const RequestInfo& request_info) const throw();

  protected:
    typedef Sync::Policy::PosixThread SyncPolicy;

    typedef std::unique_ptr<GeoIPMapping::IPMapCity2> IPMapPtr;

    typedef JsonParamProcessor<JsonProcessingContext>
      JsonRequestParamProcessor;
    typedef ReferenceCounting::SmartPtr<JsonRequestParamProcessor>
      JsonRequestParamProcessor_var;
    typedef JsonCompositeParamProcessor<JsonProcessingContext>
      JsonCompositeRequestParamProcessor;
    typedef ReferenceCounting::SmartPtr<JsonCompositeRequestParamProcessor>
      JsonCompositeRequestParamProcessor_var;

    typedef FrontendCommons::RequestParamProcessor<RequestInfo>
      RequestInfoParamProcessor;
    typedef ReferenceCounting::SmartPtr<RequestInfoParamProcessor>
      RequestInfoParamProcessor_var;
    typedef Generics::GnuHashTable<
      Generics::SubStringHashAdapter, RequestInfoParamProcessor_var>
      ParamProcessorMap;

  protected:
    bool
    parse_debug_size_param_(
      DebugAdSlotSizeMap& debug_sizes,
      const String::SubString& name,
      const std::string& value) const
      throw();

    void
    init_request_param_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      RequestInfo& request_info) const
      throw();

    void
    init_adslot_(
      AdServer::CampaignSvcs::CampaignManager::AdSlotInfo& adslot_info) const
      throw();

    void
    fill_by_user_agent_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      RequestInfo& request_info,
      String::SubString user_agent,
      bool filter_request,
      bool application = false)
      const
      throw();

    void
    fill_by_referer_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      std::string& search_words,
      const HTTP::HTTPAddress& referer,
      bool fill_search_words = true,
      bool fill_instantiate_type = true)
      const
      throw();

    void
    fill_additional_url_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      std::string& search_words,
      const HTTP::HTTPAddress& add_url)
      const
      throw();

    void
    fill_search_words_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      std::string& search_words,
      const HTTP::HTTPAddress& url)
      const
      throw();

    void
    fill_by_ip_(
      RequestInfo& request_info,
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      String::SubString ip)
      const
      throw();

    void
    fill_request_type_(
      RequestInfo& request_info,
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      const std::string& source_id)
      const
      throw();

    void
    fill_vast_instantiate_type_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      const std::string& source_id) const
      throw();

    void
    add_special_keywords_(
      std::string& keywords,
      const RequestInfo& request_info,
      const String::SubString& alt_app_id = String::SubString())
      const
      throw();

    void
    verify_user_id_(
      const std::string& signed_user_id,
      const std::string& source_id,
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params)
      const
      throw ();

    bool
    use_external_user_id_(String::SubString external_user_id)
      const throw();

    void
    select_referer_(
      AdServer::CampaignSvcs::CampaignManager::RequestParams& request_params,
      const JsonProcessingContext& context,
      HTTP::HTTPAddress& referer) const
      throw (eh::Exception);

    std::string
    first_significant_domain_part_(const String::SubString& host) const
      throw (eh::Exception);

    static
    std::string
    openrtb_ext_tag_id(
      const std::string& publisher_id,
      const std::string& id,
      const std::string& publisher_name,
      const std::string& name);

    static
    std::string
    normalize_ext_tag_id_(const String::SubString& src)
      throw();

    void
    add_param_processor_(
      const String::SubString& name,
      RequestInfoParamProcessor* processor)
      throw();

    void
    init_param_processors_() throw();

    void
    init_appnexus_processors_() throw();

    static std::string
    make_ssp_uid_by_device_(const JsonProcessingContext& ctx)
      throw (eh::Exception);

    static std::string
    adapt_app_store_url_(const String::SubString& store_url)
      throw (eh::Exception);

    static std::string
    norm_keyword_(const String::SubString& kw) throw();

  private:
    Logging::Logger_var logger_;
    unsigned long colo_id_;
    const ExternalUserIdSet skip_external_ids_;
    const CommonModule_var common_module_;
    const bool ip_logging_enabled_;
    const std::string ip_salt_;

    IPMapPtr ip_map_;
    FrontendCommons::UserAgentMatcher user_agent_matcher_;

    ParamProcessorMap param_processors_;
    JsonRequestParamProcessor_var json_root_processor_;
    JsonRequestParamProcessor_var appnexus_root_processor_;

    const SourceMap sources_;
    const bool enable_profile_referer_;
    const AccountTraitsById account_traits_;
  };
}
}

namespace Request
{
  namespace Context
  {
    extern const String::SubString SOURCE_ID;
  }
}

#endif /*BIDDINGFRONTEND_REQUESTINFOFILLER_HPP_*/
