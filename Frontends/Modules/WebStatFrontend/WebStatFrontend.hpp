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

#ifndef ADSERVER_WEBSTATFRONTEND_HPP
#define ADSERVER_WEBSTATFRONTEND_HPP

#include <string>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/FileCache.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>
#include <String/StringManip.hpp>
#include <HTTP/Http.hpp>
#include <HTTP/HTTPCookie.hpp>
//#include <Apache/Adapters.hpp>

#include <CORBA/CORBACommons/CorbaAdapters.hpp>

#include <Commons/CorbaConfig.hpp>
#include <Commons/CorbaObject.hpp>

#include <xsd/Frontends/FeConfig.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
//#include <Frontends/FrontendCommons/CookieManager.hpp>
//#include <Frontends/FrontendCommons/ServerNameFilterAdapter.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

#include "RequestInfoFiller.hpp"

namespace AdServer
{
namespace WebStat
{
  class Frontend:
    private FrontendCommons::HTTPExceptions,
    private Logging::LoggerCallbackHolder,
    public virtual FrontendCommons::FrontendInterface,
    //public Apache::ServerNameFilterAdapter<Frontend>,
    public ReferenceCounting::AtomicImpl
  {
  public:
    Frontend(
      Configuration* frontend_config,
      Logging::Logger* logger,
      CommonModule* common_module)
      throw(eh::Exception);

    virtual bool
    will_handle(const String::SubString& uri) throw();

    virtual int
    handle_request(
      const FCGI::HttpRequest& request,
      FCGI::HttpResponse& response)
      throw();

    /** Performs initialization for the module child process. */
    virtual void
    init() throw (eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw ();

  private:
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;

    typedef Configuration::FeConfig::WebStatFeConfiguration_type
      WebStatFeConfiguration;

    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<WebStatFeConfiguration> ConfigPtr;

    typedef Generics::FileCache<> FileCache;
    typedef FileCache::Cache_var FileCachePtr;

    typedef std::unique_ptr<
      Commons::CorbaObject<AdServer::CampaignSvcs::CampaignManager> >
      CampaignManagerRef;

  private:
    virtual ~Frontend() throw ();

    void parse_config_() throw(Exception);

  private:
    // configuration
    Configuration_var frontend_config_;
    CommonConfigPtr common_config_;
    ConfigPtr config_;
    CommonModule_var common_module_;

    std::unique_ptr<RequestInfoFiller> request_info_filler_;
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;

    FileCachePtr pixel_;
  };
} /*WebStat*/
} /*AdServer*/

//
// Inlines
//
namespace AdServer
{
namespace WebStat
{
  inline
  Frontend::~Frontend() throw ()
  {
  }
}
} /* AdServer */

#endif // ADSERVER_WEBSTATFRONTEND_HPP
