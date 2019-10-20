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

/// @file NullAdFrontend.hpp
#ifndef _AD_SERVER_NULL_AD_FRONTEND_HPP_
#define _AD_SERVER_NULL_AD_FRONTEND_HPP_

#include <string>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/ActiveObject.hpp>
#include <Logger/Logger.hpp>
#include <Logger/StreamLogger.hpp>
#include <String/StringManip.hpp>
#include <HTTP/Http.hpp>
#include <Frontends/FrontendCommons/ProcessingTemplates.hpp>

#include <CORBA/CORBACommons/CorbaAdapters.hpp>
#include <CORBA/CORBACommons/ObjectPool.hpp>

#include <Commons/CorbaConfig.hpp>

#include <CampaignSvcs/CampaignServer/CampaignServer.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>
#include <CampaignSvcs/CampaignManager/PassbackTemplate.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>
#include <Frontends/CommonModule/CommonModule.hpp>

#include <xsd/Frontends/FeConfig.hpp>

namespace AdServer
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class NullAdFrontend:
    public virtual FrontendCommons::FrontendInterface,
    public virtual ReferenceCounting::AtomicImpl,
    private Logging::LoggerCallbackHolder
  {
  public:
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;
    typedef Configuration::FeConfig::NullAdFeConfiguration_type
      NullAdFeConfiguration;

  public:
    NullAdFrontend(
      Configuration* frontend_config,
      Logging::Logger* logger)
      throw (eh::Exception);

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
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

    struct RequestInfo
    {
      RequestInfo() 
      : tag_id(0),
        request_app_format("html"),
        passback_type("html"),
        secure(false),
        random(0)
      {}

      unsigned long tag_id;
      std::string request_app_format;
      std::string passback_type;
      std::string passback_url;
      std::string passback_code;
      unsigned long width;
      unsigned long height;
      std::string size;
      bool secure;
      unsigned long random;
    };

    typedef
      CORBACommons::ObjectPoolRefConfiguration CampaignServerPoolConfig;

    typedef
      CORBACommons::ObjectPool<
        AdServer::CampaignSvcs::CampaignServer,
        CampaignServerPoolConfig>
      CampaignServerPool;

    typedef std::unique_ptr<CampaignServerPool> CampaignServerPoolPtr;

    typedef std::map<std::string, std::string> TokenValueMap;

    struct TemplateRule
    {
      TokenValueMap tokens;
    };

    typedef std::list<std::string> StringList;

    typedef Generics::GnuHashTable<
      Generics::SubStringHashAdapter, TemplateRule>
      TemplateRuleMap;

  private:
    virtual
    ~NullAdFrontend() throw () {};

    CampaignServerPoolPtr
    resolve_campaign_servers_()
      throw(Exception, eh::Exception);

    void parse_param_(
      const String::SubString& name,
      const String::SubString& value,
      RequestInfo& info)
      throw(FrontendCommons::HTTPExceptions::InvalidParamException);

  private:
    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<NullAdFeConfiguration> ConfigPtr;

    CommonConfigPtr common_config_;
    ConfigPtr config_;
    Configuration_var frontend_config_;

    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    CampaignServerPoolPtr campaign_servers_;

    StringList strings_; // string's holder for SubString using
    TemplateRuleMap template_rules_;

    AdServer::CampaignSvcs::PassbackTemplateMap passback_templates_;
  };
}

#endif // _AD_SERVER_NULL_AD_FRONTEND_HPP_
