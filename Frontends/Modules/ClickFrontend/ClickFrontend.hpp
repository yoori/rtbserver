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


#ifndef _AD_FRONTENDS_CLICK_FRONTEND_CLICK_FRONTEND_HPP_
#define _AD_FRONTENDS_CLICK_FRONTEND_CLICK_FRONTEND_HPP_

#include <string>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/Uuid.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>

#include <HTTP/Http.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <ChannelSvcs/ChannelManagerController/ChannelServerSessionFactory.hpp>

#include <Frontends/CommonModule/CommonModule.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Commons/TextTemplateCache.hpp>
#include <Frontends/FrontendCommons/ChannelServerSessionPool.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/UserBindClient.hpp>
#include <Frontends/FrontendCommons/FCGI.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

#include <xsd/Frontends/FeConfig.hpp>

#include "RequestInfoFiller.hpp"


namespace AdServer
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class ClickFrontend:
    public FrontendCommons::FrontendInterface,
    private FrontendCommons::HTTPExceptions,
    private Logging::LoggerCallbackHolder,
    private virtual Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    typedef FrontendCommons::HTTPExceptions::Exception Exception;

  public:
    ClickFrontend(
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

    virtual int
    handle_request_noparams(
      FCGI::HttpRequest& request,
      FCGI::HttpResponse& response)
      throw (eh::Exception);

    /** Performs initialization for the module child process. */
    virtual void
    init() throw (eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw ();

    bool
    log(const String::SubString& text,
      unsigned long severity = Logging::Logger::INFO,
      const char* aspect = 0,
      const char* error_code = 0) const
      throw ();

    unsigned long
    log_level() throw ();

  private:
    class MatchClickChannelsTask;
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::ClickFeConfiguration_type
      ClickFeConfiguration;

    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<ClickFeConfiguration> ConfigPtr;

    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

  private:
    virtual
    ~ClickFrontend() throw();

    void
    parse_config_() throw(Exception);

    void
    check_constraints_(
      const FrontendCommons::ParsedParamsMap& params,
      const FCGI::HttpRequest& request) const
      throw (ForbiddenException, InvalidParamException);

    void
    resolve_channel_session_() throw (Exception);

    void
    match_click_channels_(
      const AdServer::Commons::UserId& user_id,
      const AdServer::Commons::UserId& cookie_user_id,
      const Generics::Time& now,
      ::CORBA::ULong campaign_id,
      ::CORBA::ULong advertiser_id,
      const String::SubString& peer_ip,
      const std::list<std::string>& markers)
      throw ();

    void
    fill_match_request_info_(
      AdServer::CampaignSvcs::CampaignManager::MatchRequestInfo& mri,
      const AdServer::Commons::UserId& user_id,
      const Generics::Time& now,
      const AdServer::ChannelSvcs::ChannelServerBase::MatchResult* trigger_match_result,
      const AdServer::UserInfoSvcs::UserInfoMatcher::MatchResult* history_match_result,
      const String::SubString& peer_ip_val) const
      throw ();

  private:
    std::string config_file_;

    /* configuration */
    CommonConfigPtr common_config_;
    ConfigPtr config_;
    Configuration_var frontend_config_;

    CommonModule_var common_module_;

    std::unique_ptr<ClickFE::RequestInfoFiller> request_info_filler_;

    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    ChannelServerSessionFactoryImpl_var server_session_factory_;
    std::unique_ptr<FrontendCommons::ChannelServerSessionPool>
      channel_servers_;
    FrontendCommons::UserBindClient_var user_bind_client_;
    FrontendCommons::UserInfoClient_var user_info_client_;

    Generics::StringHashAdapter click_template_file_;
    Commons::TextTemplateCache_var template_files_;

    typedef std::unique_ptr<GeoIPMapping::IPMapCity2> IPMapPtr;
    IPMapPtr ip_map_;

    Generics::TaskRunner_var task_runner_;

    std::unique_ptr<FrontendCommons::CookieManager<
      FCGI::HttpRequest, FCGI::HttpResponse> > cookie_manager_;
    SetUidController_var set_uid_controller_;
  };

  typedef ReferenceCounting::QualPtr<ClickFrontend> ClickFrontend_var;
}

//
// Inlines
//
namespace AdServer
{
  inline
  ClickFrontend::~ClickFrontend() throw ()
  {}

  inline
  unsigned long
  ClickFrontend::log_level() throw ()
  {
    if (!logger())
    {
      return 0;
    }

    return config_->Logger().log_level();
  }
} // namespace AdServer

#endif // _AD_FRONTENDS_CLICK_FRONTEND_CLICK_FRONTEND_HPP_
