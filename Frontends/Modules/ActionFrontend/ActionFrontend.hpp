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


#ifndef _AD_FRONTENDS_ACTION_FRONTEND_ACTION_FRONTEND_HPP_
#define _AD_FRONTENDS_ACTION_FRONTEND_ACTION_FRONTEND_HPP_

#include <string>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>
#include <Generics/FileCache.hpp>
#include <Generics/Uuid.hpp>
#include <GeoIP/IPMap.hpp>
#include <String/TextTemplate.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <Commons/TextTemplateCache.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Frontends/FrontendCommons/UserBindClient.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Frontends/FrontendCommons/ChannelServerSessionPool.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

#include <xsd/Frontends/FeConfig.hpp>

#include "ActionFrontendStat.hpp"
#include "RequestInfoFiller.hpp"

namespace AdServer
{
namespace Action
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class Frontend:
    private FrontendCommons::HTTPExceptions,
    private Logging::LoggerCallbackHolder,
    private Generics::CompositeActiveObject,
    public virtual FrontendCommons::FrontendInterface,
    public virtual ReferenceCounting::AtomicImpl
  {
    typedef FrontendCommons::HTTPExceptions::Exception Exception;

  public:
    typedef ReferenceCounting::SmartPtr<Frontend> Frontend_var;
    static Frontend_var instance;

    Frontend(
      Configuration* frontend_config,
      Logging::Logger* logger,
      CommonModule* common_module)
      throw (eh::Exception);

    /** Determines whether the module is able to process the URI.
     *
     * @param uri URI provided by Apache.
     */
    virtual bool
    will_handle(const String::SubString& uri) throw ();

    /** Executed if will_handle returns true; processes an HTTP request. Provides
     *  browsing context to a Channel Manager, returns list of channels in the HTTP
     *  response.
     *
     * @param uri The URI for assessing.
     * @param params HTTP request parameters.
     * @param headers HTTP request headers. This is an in/out argument; handle_request may also
     * return headers.
     * @param cookies HTTP cookies. This is an in/out argument; handle_request may also
     * set cookies.
     * @param istr Input stream to read the HTTP request body from.
     * @param ostr Output stream to write the HTTP response body to.
     *
     * @return HTTP status code.
     */
    virtual int
    handle_request(
      const FCGI::HttpRequest& request,
      FCGI::HttpResponse& response) throw ();

    /** Performs initialization for the module child process. */
    virtual void
    init() throw (eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw ();

  private:
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::ActionFeConfiguration_type
      ActionFeConfiguration;

    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

    typedef Generics::FileCache<> FileCache;
    typedef FileCache::Cache_var FileCachePtr;

    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<ActionFeConfiguration> ConfigPtr;
    typedef std::unique_ptr<GeoIPMapping::IPMap> IPMapPtr;

    struct DerivedConfig
    {
      xsd::AdServer::Configuration::UriListType::Uri_sequence advertiser_service_uri;
      Commons::LogReferrer::Setting use_referrer;
    };

    struct RedirectRule: public ReferenceCounting::AtomicImpl
    {
    public:
      bool use_keywords;
      Generics::GnuHashSet<Generics::StringHashAdapter> keywords;
      Commons::TextTemplate_var url_template;

    protected:
      virtual
      ~RedirectRule() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<RedirectRule> RedirectRule_var;

    typedef std::vector<RedirectRule_var> RedirectRuleArray;

  private:
    virtual
    ~Frontend() throw ();

    void
    parse_config_() throw(Exception);

    void
    write_pixel(FCGI::HttpResponse& response)
      throw (eh::Exception);

    void
    write_html(FCGI::HttpResponse& response)
      throw (eh::Exception);

    int
    process_advertiser_request(
      FCGI::HttpResponse& response,
      const FCGI::HttpRequest& request,
      const RequestInfo& request_info,
      bool return_html)
      throw (Exception, eh::Exception);

    void
    action_taken_all_(
      const RequestInfo& request_info,
      const Commons::UserId& utm_cookie_resolved_user_id)
      throw();

    void
    trigger_match_all_(
      const RequestInfo& request_info,
      const Commons::UserId& utm_cookie_resolved_user_id)
      throw();

    void
    relink_user_id_all_(
      const RequestInfo& request_info,
      const Commons::UserId& utm_cookie_resolved_user_id,
      const Commons::UserId& link_user_id)
      throw();

    void
    trigger_match_(
      AdServer::ChannelSvcs::ChannelServerBase::MatchResult_var& trigger_match_result,
      unsigned long conv_id,
      const Generics::Time& now,
      const AdServer::Commons::UserId& user_id,
      const String::SubString& referer)
      throw();

    bool
    resolve_user_id_(
      Commons::UserId& result_user_id,
      const String::SubString& external_user_id,
      const Commons::UserId& current_user_id,
      const Generics::Time& time)
      throw();

    void
    fill_match_request_info_(
      AdServer::CampaignSvcs::CampaignManager::MatchRequestInfo& mri,
      const AdServer::Commons::UserId& user_id,
      const Generics::Time& now,
      const AdServer::ChannelSvcs::ChannelServerBase::MatchResult_var& trigger_match_result) const
      throw ();

    class MatchActionChannelsTask;

    typedef std::unique_ptr<FrontendCommons::CookieManager<
      FCGI::HttpRequest, FCGI::HttpResponse> >
      CookieManagerPtr;

  private:
    // configuration
    //std::string config_file_;
    CommonConfigPtr common_config_;
    ConfigPtr config_;
    Configuration_var frontend_config_;

    DerivedConfig derived_config_;
    CommonModule_var common_module_;

    std::unique_ptr<AdServer::Action::RequestInfoFiller> request_info_filler_;
    ChannelServerSessionFactoryImpl_var server_session_factory_;
    std::unique_ptr<FrontendCommons::ChannelServerSessionPool>
      channel_servers_;
    FrontendCommons::UserInfoClient_var user_info_client_;
    CookieManagerPtr cookie_manager_;

    IPMapPtr ip_map_;
    FileCachePtr track_pixel_;
    FileCachePtr track_html_;
    AcFrontendStat_var stats_;
    RedirectRuleArray redirect_rules_;

    Generics::TaskRunner_var task_runner_;

    /* external services */
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    FrontendCommons::UserBindClient_var user_bind_client_;
  };
}
}

namespace AdServer
{
namespace Action
{
  /* Frontend class */
  inline
  Frontend::~Frontend() throw ()
  {}
}
} // namespace AdServer

#endif // _AD_FRONTENDS_ACTION_FRONTEND_ACTION_FRONTEND_HPP_
