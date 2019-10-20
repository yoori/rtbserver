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


#ifndef AD_SERVER_CHANNEL_SERVICE_CHANNEL_FRONTEND_CHANNEL_FRONTEND_MODULE_HPP_
#define AD_SERVER_CHANNEL_SERVICE_CHANNEL_FRONTEND_CHANNEL_FRONTEND_MODULE_HPP_

#include <string>
#include <vector>
#include <list>

#include <eh/Exception.hpp>

#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>

#include <Generics/ActiveObject.hpp>
#include <Generics/Statistics.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/Uuid.hpp>
#include <Sync/PosixLock.hpp>

#include <HTTP/Http.hpp>
#include <HTTP/HTTPCookie.hpp>

#include <CORBACommons/CorbaAdapters.hpp>

#include <UserInfoSvcs/UserInfoManagerController/UserInfoManagerController.hpp>
#include <Frontends/FrontendCommons/UserBindClient.hpp>
#include <Frontends/FrontendCommons/ChannelServerSessionPool.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Frontends/FrontendCommons/TaskScheduler.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>
#include <Frontends/FrontendCommons/FCGI.hpp>
#include <Frontends/CommonModule/CommonModule.hpp>

#include <xsd/Frontends/FeConfig.hpp>

#include "AdFrontendStat.hpp"
#include "DebugSink.hpp"
#include "RequestInfoFiller.hpp"

namespace AdServer
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class AdFrontend:
    public FrontendCommons::FrontendInterface,
    private FrontendCommons::HTTPExceptions,
    private Logging::LoggerCallbackHolder,
    private virtual Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
    typedef FrontendCommons::HTTPExceptions::Exception Exception;

  public:
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::AdFeConfiguration_type
      AdFeConfiguration;

    typedef Configuration::FeConfig::PassFeConfiguration_type
      PassFeConfiguration;

    typedef FCGI::HttpResponse HttpResponse;

    AdFrontend(
      Configuration* frontend_config,
      Logging::Logger* logger,
      CommonModule* common_module)
      throw(eh::Exception);

    /** Determines whether the module is able to process the URI.
     *
     * @param uri URI provided by Apache.
     */
    virtual bool
    will_handle(const String::SubString& uri) throw ();

    /**
     * Executed if will_handle returns true; processes an HTTP request. Provides
     * browsing context to a Channel Manager, returns list of channels in the HTTP
     * response.
     * @param request HTTP request.
     * @param response The object to write the HTTP response body.
     * @return HTTP status code.
     */
    virtual int
    handle_request(const FCGI::HttpRequest& request,
      HttpResponse& response) throw ();

    /** Performs initialization for the module child process. */
    virtual void
    init() throw (eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void shutdown() throw ();

    /**
     * Update ColoFlagsMap from CampaignManager
     */
    void
    update_colocation_flags() throw ();

    static AdServer::ChannelSvcs::ChannelServerBase::MatchResult*
    get_empty_trigger_matching()
      throw(eh::Exception);

    static AdServer::ChannelSvcs::ChannelServerBase::MatchQuery*
    get_empty_matching_query()
      throw(eh::Exception);

    static AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult*
    get_empty_history_matching()
      throw(eh::Exception);

  protected:
    virtual ~AdFrontend() throw () {}

  private:

    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<AdFeConfiguration> ConfigPtr;
    typedef std::unique_ptr<PassFeConfiguration> PassConfigPtr;
    typedef std::unique_ptr<
      FrontendCommons::CookieManager<FCGI::HttpRequest, HttpResponse> >
      CookieManagerPtr;

    typedef Sync::Policy::PosixThreadRW SyncPolicy;

  private:
    void parse_configs_() throw(Exception);

    void sign_client_id(
      const std::string& uid,
      std::string& signed_uid)
      throw();

    void
    start_update_loop_() throw (Exception);

    bool
    resolve_cookie_user_id_(
      AdServer::Commons::UserId& resolved_user_id,
      const RequestInfo& request_info)
      throw();

    int
    acquire_ad(
      HttpResponse& response,
      const FCGI::HttpRequest& request,
      const RequestInfo& request_info,
      const Generics::SubStringHashAdapter& instantiate_creative_type,
      std::string& str_response,
      PassbackInfo& passback_info,
      bool& log_as_test,
      DebugSink* debug_sink,
      RequestTimeMetering& request_time_metering)
      throw (Exception);

    void
    request_campaign_manager_(
      PassbackInfo& passback_info,
      bool& log_as_test,
      AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult_var&
        campaign_matching_result,
      RequestTimeMetering& request_time_metering,
      const RequestInfo& request_info,
      const Generics::SubStringHashAdapter& instantiate_type,
      AdServer::ChannelSvcs::ChannelServerBase::MatchResult*
        trigger_matched_channels,
      AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult* history_match_result,
      const Generics::Time& merged_last_request,
      bool profiling_available,
      const AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq* ccg_keywords,
      const AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq* hid_ccg_keywords,
      DebugSink* debug_sink)
      throw (Exception);

    void
    convert_ccg_keywords_(
      AdServer::CampaignSvcs::CampaignManager::CCGKeywordSeq& ccg_keywords,
      const AdServer::ChannelSvcs::ChannelServerBase::CCGKeywordSeq* src_ccg_keywords)
      throw();

    void
    merge_users(
      RequestTimeMetering& time_metering,
      bool& merge_success,
      Generics::Time& last_request,
      std::string& merge_error_message,
      const RequestInfo& request_info)
      throw();

    void
    match_triggers_(
      RequestTimeMetering& request_time_metering,
      AdServer::ChannelSvcs::ChannelServerBase::MatchQuery* query,
      AdServer::ChannelSvcs::ChannelServerBase::MatchResult_out
        trigger_matched_channels,
      const RequestInfo& request_info)
      throw(Exception);

    void
    acquire_user_info_matcher(
      const RequestInfo& request_info,
      const AdServer::ChannelSvcs::ChannelServerBase::MatchResult*
        trigger_matching_result,
      AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult_out match_result_out,
      bool& profiling_available,
      RequestTimeMetering& request_time_metering)
      throw ();

    void
    user_info_post_match_(
      RequestTimeMetering& request_time_metering,
      const RequestInfo& request_info,
      const AdServer::CampaignSvcs::CampaignManager::RequestCreativeResult&
        campaign_select_result)
      throw();

    void
    opt_out_client_(
      const HTTP::CookieList& cookies,
      HttpResponse& response,
      const FCGI::HttpRequest& request,
      const RequestInfo& request_info)
      throw();

    static void
    fill_debug_channels_(
      const AdServer::ChannelSvcs::ChannelServerBase::ChannelAtomSeq& in,
      char type,
      DebugStream& out)
      throw(eh::Exception);

  private:
    void
    log_request(
      const char* function_name,
      const FCGI::HttpRequest& request,
      unsigned int log_level)
      throw (eh::Exception);

    static void prepare_ui_match_params_(
      AdServer::UserInfoSvcs::UserInfoMatcher::MatchParams& match_params,
      const AdServer::ChannelSvcs::ChannelServerBase::MatchResult* match_result,
      const RequestInfo& request_info)
      throw(eh::Exception);

  private:
    /* configuration */
    CommonConfigPtr common_config_;
    ConfigPtr config_;
    PassConfigPtr pass_config_;
    std::string fe_config_path_;
    Configuration_var frontend_config_;

    CommonModule_var common_module_;

    std::unique_ptr<RequestInfoFiller> request_info_filler_;

    CookieManagerPtr cookie_manager_;
    std::list<std::string> remove_cookies_holder_;
    FrontendCommons::CookieNameSet remove_cookies_;

    /* external services */
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    std::unique_ptr<FrontendCommons::ChannelServerSessionPool> channel_servers_;
    
    Generics::TaskRunner_var task_runner_;
    FrontendCommons::TaskScheduler_var task_scheduler_;
    FrontendCommons::UserBindClient_var user_bind_client_;
    FrontendCommons::UserInfoClient_var user_info_client_;

    AdFrontendStat_var stats_;
  };
}

#endif // AD_SERVER_CHANNEL_SERVICE_CHANNEL_FRONTEND_CHANNEL_FRONTEND_MODULE_HPP_
