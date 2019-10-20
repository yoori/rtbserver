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

#ifndef ADSERVER_ADINSTFRONTEND_HPP
#define ADSERVER_ADINSTFRONTEND_HPP

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

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Frontends/FrontendCommons/TaskScheduler.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Frontends/FrontendCommons/FCGI.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

#include <xsd/Frontends/FeConfig.hpp>

#include "RequestInfoFiller.hpp"

namespace AdServer
{
namespace Instantiate
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
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::AdInstFeConfiguration_type
      AdInstFeConfiguration;

    typedef FCGI::HttpResponse HttpResponse;

    Frontend(
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

    virtual int
    handle_request_noparams(
      FCGI::HttpRequest& request,
      HttpResponse& response)
      throw (eh::Exception);

    /** Performs initialization for the module child process. */
    virtual void
    init() throw (eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw ();

  protected:
    virtual ~Frontend() throw ();

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
    typedef std::unique_ptr<AdInstFeConfiguration> ConfigPtr;
    typedef std::unique_ptr<
      FrontendCommons::CookieManager<FCGI::HttpRequest, FCGI::HttpResponse> >
      CookieManagerPtr;

  private:
    void
    parse_configs_() throw(Exception);

    void
    merge_users_(
      bool& merge_success,
      std::string& merge_error_message,
      const RequestInfo& request_info)
      throw();

    int
    instantiate_click_(
      HttpResponse& response,
      const RequestInfo& request_info,
      const AdServer::CampaignSvcs::CampaignManager::InstantiateAdResult*
        inst_ad_result)
      throw (Exception);

    int
    instantiate_ad_(
      HttpResponse& response,
      const RequestInfo& request_info,
      const Generics::SubStringHashAdapter& instantiate_creative_type)
      throw (Exception);

    void
    log_request_(
      const char* function_name,
      const FCGI::HttpRequest& request,
      unsigned int log_level)
      throw (eh::Exception);

  private:
    CommonConfigPtr common_config_;
    ConfigPtr config_;
    //std::string fe_config_path_;
    Configuration_var frontend_config_;

    CommonModule_var common_module_;

    std::unique_ptr<RequestInfoFiller> request_info_filler_;
    CookieManagerPtr cookie_manager_;

    /* external services */
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    FrontendCommons::UserInfoClient_var user_info_client_;
  };
}
}

// Inlines
namespace AdServer
{
namespace Instantiate
{
  inline
  Frontend::~Frontend() throw ()
  {}
}
}

#endif /*ADSERVER_ADINSTFRONTEND_HPP*/
