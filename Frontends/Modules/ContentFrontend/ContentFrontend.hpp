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

#ifndef FRONTENDS_CONTENTFRONTEND_HPP
#define FRONTENDS_CONTENTFRONTEND_HPP

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
// #include <Frontends/FrontendCommons/CookieManager.hpp>
#include <Commons/TextTemplateCache.hpp>
#include <Frontends/FrontendCommons/CampaignManagersPool.hpp>
#include <Frontends/FrontendCommons/HTTPExceptions.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

namespace AdServer
{
  class ContentFrontend
    : public FrontendCommons::HTTPExceptions,
      private Logging::LoggerCallbackHolder,
      public virtual FrontendCommons::FrontendInterface,
      public ReferenceCounting::AtomicImpl
  {
  public:
    typedef Configuration::FeConfig::CommonFeConfiguration_type
      CommonFeConfiguration;
    typedef Configuration::FeConfig::ContentFeConfiguration_type
      ContentFeConfiguration;

    typedef ReferenceCounting::SmartPtr<ContentFrontend>
      ContentFrontend_var;

  public:
    static ContentFrontend_var instance;

  public:
    ContentFrontend(
      Configuration* frontend_config,
      Logging::Logger* logger)
      throw (eh::Exception);

    virtual void
    init() throw(eh::Exception);

    virtual bool
    will_handle(
      const String::SubString& uri) throw ();

    virtual int
    handle_request_noparams(
      FCGI::HttpRequest& request,
      FCGI::HttpResponse& response)
      throw (eh::Exception);

    virtual int
    handle_request(
      const FCGI::HttpRequest& request,
      FCGI::HttpResponse& response) throw ();

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw ();

    bool
    log(const String::SubString& text,
      unsigned long severity = Logging::Logger::INFO,
      const char* aspect = 0,
      const char* error_code = 0) const
      throw ();

  private:
    typedef Commons::TextTemplateCacheConfiguration<
      Commons::TextTemplate>::Exception Exception;

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
    typedef std::unique_ptr<ContentFeConfiguration> ConfigPtr;

    typedef std::map<std::string, std::string> TokenValueMap;

    struct TemplateRule
    {
      TokenValueMap tokens;
      std::string resource_url_prefix;
    };

    typedef std::list<std::string> StringList;

    typedef Generics::GnuHashTable<
      Generics::SubStringHashAdapter, TemplateRule>
      TemplateRuleMap;

  private:
    class CreativesUpdater :
      public Commons::TextTemplateCacheConfiguration<
        Commons::TextTemplate>::FarUpdater
    {
      typedef Commons::TextTemplateCacheConfiguration<
        Commons::TextTemplate> ConfigType;
      typedef ConfigType::Holder Holder;
      typedef ConfigType::Exception Exception;

      virtual ~CreativesUpdater() throw () {}

      FrontendCommons::CampaignManagersPool<Exception>& campaign_managers_;
    public:

      CreativesUpdater(FrontendCommons::CampaignManagersPool<Exception>& campaign_managers)
        throw ()
        : campaign_managers_(campaign_managers)
      {}

      virtual Holder
      far_update(const char* file, const char* service_index) throw (Exception)
      {
        CORBACommons::OctSeq_var content;
        campaign_managers_.get_file(file, content, service_index);
        try
        {
          String::SubString file_body(reinterpret_cast<char*>(content->get_buffer()), content->length());
          Generics::Time now = Generics::Time::get_time_of_day();
          return new ConfigType::TextTemplateHolder(
            Commons::TextTemplate_var(new Commons::TextTemplate(file_body)),
            now,
            now,
            file_body.size());
        }
        catch (const eh::Exception& e)
        {
          Stream::Error ostr;
          ostr << "CreativesUpdater::far_update(): caugth eh::Exception: "
            << e.what();
          throw Exception(ostr);
        }
      }
    };

    virtual ~ContentFrontend() throw () {};

    void parse_configs_() throw(Exception);

    void parse_headers_(
      const FCGI::HttpRequest& request,
      bool& secure) throw ();

  private:
    Configuration_var frontend_config_;
    CommonConfigPtr common_config_;
    ConfigPtr config_;

    StringList strings_; // string's holder for SubString using
    TemplateRuleMap template_rules_;

    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    FrontendCommons::CampaignManagersPool<Exception> campaign_managers_;
    Commons::TextTemplateCache_var template_files_;
  };


}

#endif /*FRONTENDS_CONTENTFRONTEND_HPP*/
