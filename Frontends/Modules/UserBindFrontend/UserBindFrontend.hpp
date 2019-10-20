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

#ifndef ADSERVER_USERBINDFRONTEND_HPP
#define ADSERVER_USERBINDFRONTEND_HPP

#include <eh/Exception.hpp>

#include <GeoIP/IPMap.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>

#include <Generics/ActiveObject.hpp>
#include <Generics/FileCache.hpp>
#include <Generics/Uuid.hpp>

#include <HTTP/Http.hpp>
#include <HTTP/HTTPCookie.hpp>

#include <String/TextTemplate.hpp>

#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ObjectPool.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/UserBindClient.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Frontends/FrontendCommons/ChannelServerSessionPool.hpp>

#include <Frontends/FrontendCommons/FrontendInterface.hpp>

#include <xsd/Frontends/FeConfig.hpp>

#include "RequestInfoFiller.hpp"

namespace AdServer
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class UserBindFrontend;

  typedef ReferenceCounting::SmartPtr<UserBindFrontend> UserBindFrontend_var;

  class UserBindFrontend:
    private FrontendCommons::HTTPExceptions,
    private Logging::LoggerCallbackHolder,
    public virtual FrontendCommons::FrontendInterface,
    private virtual Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
    typedef FrontendCommons::HTTPExceptions::Exception Exception;
    DECLARE_EXCEPTION(InvalidSource, eh::DescriptiveException);

  public:
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::UserBindFeConfiguration_type
      UserBindFeConfiguration;

  public:
    UserBindFrontend(
      Configuration* frontend_config,
      Logging::Logger* logger,
      CommonModule* common_module)
      throw(eh::Exception);

    virtual bool
    will_handle(const String::SubString& uri) throw ();

    virtual int
    handle_request(
      const FCGI::HttpRequest& request,
      FCGI::HttpResponse& response) throw();

    /** Performs initialization for the module child process. */
    virtual void
    init() throw(eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw();

  protected:
    class RequestTask;
    typedef ReferenceCounting::SmartPtr<RequestTask>
      RequestTask_var;

    class UserMatchTask;

    struct BindResult;
    struct BindResultHolder;

  protected:
    virtual ~UserBindFrontend() throw();

    int
    handle_request_(
      const FCGI::HttpRequest& request,
      FCGI::HttpResponse& response)
      throw();

    int
    handle_delete_request_(
      const UserBind::RequestInfo& request_info)
      throw();

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
    typedef std::unique_ptr<UserBindFeConfiguration> ConfigPtr;

    typedef CORBACommons::ObjectPoolRefConfiguration
      UserBindServerPoolConfig;
    typedef CORBACommons::ObjectPool<
      AdServer::UserInfoSvcs::UserBindServer, UserBindServerPoolConfig>
      UserBindServerPool;
    typedef std::unique_ptr<UserBindServerPool> UserBindServerPoolPtr;
    typedef UserBindServerPool::ObjectHandlerType UserBindServerHandler;

    struct RedirectRule: public ReferenceCounting::AtomicImpl
    {
    public:
      bool use_keywords;
      bool passback;
      Generics::GnuHashSet<Generics::StringHashAdapter> keywords;
      String::TextTemplate::IStream redirect;

      bool init_bind_request;

    protected:
      virtual
      ~RedirectRule() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<RedirectRule> RedirectRule_var;

    class SourceEntity: public ReferenceCounting::AtomicImpl
    {
    public:
      std::list<RedirectRule_var> rules;

    protected:
      virtual
      ~SourceEntity() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<SourceEntity> SourceEntity_var;
    typedef std::map<std::string, SourceEntity_var> SourceMap;

    typedef Generics::FileCache<> FileCache;
    typedef FileCache::Cache_var FileCachePtr;

  private:
    void
    parse_configs_() throw(Exception);

    int
    process_request_(
      BindResultHolder& result_holder,
      const UserBind::RequestInfo& request_info,
      const String::SubString& dns_bind_request_id)
      throw();

    uint32_t
    calc_yandex_sign_(
      const UserBind::RequestInfo& request_info,
      const std::string& user_data,
      const std::string& secure_key)
      throw ();

    void
    report_bad_user_(
      const UserBind::RequestInfo& request_info)
      throw();

    void
    log_cookie_mapping_(const UserBind::RequestInfo& request_info) throw ();

    static RedirectRule_var
    init_redirect_rule_(
      const String::SubString& redirect,
      const String::SubString* keywords,
      bool passback)
      throw(UserBindFrontend::InvalidSource);

    void
    user_match_(
      const Commons::UserId& result_user_id,
      const Commons::UserId& merge_user_id,
      bool create_user_profile,
      const String::SubString& keywords,
      const String::SubString& cohort,
      const String::SubString& referer,
      unsigned long colo_id)
      throw();

  private:
    // configuration
    CommonConfigPtr common_config_;
    ConfigPtr config_;
    Configuration_var frontend_config_;

    CommonModule_var common_module_;

    FileCachePtr pixel_;
    std::string fe_config_path_;
    std::string pixel_content_type_;
    SourceMap sources_;
    std::unique_ptr<GeoIPMapping::IPMapCity2> ip_map_;
    std::unique_ptr<UserBind::RequestInfoFiller> request_info_filler_;

    // external services
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::UserBindClient_var user_bind_client_;
    FrontendCommons::UserInfoClient_var user_info_client_;
    ChannelServerSessionFactoryImpl_var server_session_factory_;
    std::unique_ptr<FrontendCommons::ChannelServerSessionPool>
      channel_servers_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    std::unique_ptr<FrontendCommons::CookieManager<
      FCGI::HttpRequest, FCGI::HttpResponse> > cookie_manager_;

    Generics::TaskRunner_var bind_task_runner_;
    Generics::TaskRunner_var match_task_runner_;
  };
}

// Inlines
namespace AdServer
{
  inline
  UserBindFrontend::~UserBindFrontend() throw ()
  {}
}

#endif // ADSERVER_USERBINDFRONTEND_HPP
