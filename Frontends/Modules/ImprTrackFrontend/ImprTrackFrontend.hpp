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


#ifndef _AD_FRONTENDS_IMPR_TRACK_FRONTEND_IMPR_TRACK_FRONTEND_HPP_
#define _AD_FRONTENDS_IMPR_TRACK_FRONTEND_IMPR_TRACK_FRONTEND_HPP_

#include <string>
#include <set>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>
#include <Generics/FileCache.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <Commons/UserInfoManip.hpp>
#include <Commons/Containers.hpp>
#include <Commons/TextTemplateCache.hpp>
#include <UserInfoSvcs/UserInfoManagerController/UserInfoManagerController.hpp>
#include <UserInfoSvcs/UserBindServer/UserBindServer.hpp>

#include <xsd/Frontends/FeConfig.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/UserBindClient.hpp>
#include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>
#include <Frontends/FrontendCommons/FCGI.hpp>

#include "RequestInfoFiller.hpp"

namespace AdServer
{
namespace ImprTrack
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class Frontend:
    public FrontendCommons::FrontendInterface,
    private FrontendCommons::HTTPExceptions,
    private Logging::LoggerCallbackHolder,
    private virtual Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
    typedef FrontendCommons::HTTPExceptions::Exception Exception;

  public:
    Frontend(
      Configuration* frontend_config,
      Logging::Logger* logger,
      CommonModule* common_module)
      throw(eh::Exception);

    virtual bool
    will_handle(const String::SubString& uri) throw ();

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

    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::ImprTrackFeConfiguration_type
      ImprTrackFeConfiguration;

    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<ImprTrackFeConfiguration> ConfigPtr;

    typedef CORBACommons::ObjectPoolRefConfiguration
      UserBindServerPoolConfig;
    typedef CORBACommons::ObjectPool<
      AdServer::UserInfoSvcs::UserBindServer, UserBindServerPoolConfig>
      UserBindServerPool;
    typedef std::unique_ptr<UserBindServerPool> UserBindServerPoolPtr;
    typedef UserBindServerPool::ObjectHandlerType UserBindServerHandler;

    struct BindURLRule: public ReferenceCounting::AtomicImpl
    {
      bool use_keywords;
      Generics::GnuHashSet<Generics::StringHashAdapter> keywords;
      Commons::TextTemplate_var url_template;

    protected:
      virtual ~BindURLRule() throw()
      {};
    };

    typedef ReferenceCounting::SmartPtr<BindURLRule> BindURLRule_var;
    typedef std::vector<BindURLRule_var> BindURLRuleArray;

    typedef std::vector<Commons::TextTemplate_var>
      TextTemplateArray;

  private:
    virtual ~Frontend() throw ();

    void
    parse_config_() throw(Exception);

    RequestInfoFiller::EncryptionKeys_var
    read_keys_(
      const xsd::AdServer::Configuration::EncryptionKeysType& src)
      throw (eh::Exception);

    void
    report_bad_user_(const RequestInfo& request_info)
      throw();

  private:
    // configuration
    std::string config_file_;

    CommonConfigPtr common_config_;
    ConfigPtr config_;
    Configuration_var frontend_config_;

    CommonModule_var common_module_;

    std::unique_ptr<RequestInfoFiller> request_info_filler_;
    std::unique_ptr<
      FrontendCommons::CookieManager<FCGI::HttpRequest, FCGI::HttpResponse> > cookie_manager_;
    FileCachePtr track_pixel_;
    std::string track_pixel_content_type_;
    BindURLRuleArray bind_url_rules_;

    // external services
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;

    FrontendCommons::UserBindClient_var user_bind_client_;
    FrontendCommons::UserInfoClient_var user_info_client_;

    Generics::StringHashAdapter track_template_file_;
    Commons::TextTemplateCache_var template_files_;
  };
}
}

// Inlines
namespace AdServer
{
namespace ImprTrack
{
  inline
  Frontend::~Frontend() throw ()
  {}
}
} // namespace AdServer

#endif // _AD_FRONTENDS_IMPR_TRACK_FRONTEND_IMPR_TRACK_FRONTEND_HPP_
