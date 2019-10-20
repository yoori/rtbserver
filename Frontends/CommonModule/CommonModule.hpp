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

#pragma once

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/TaskRunner.hpp>
#include <Language/SegmentorCommons/SegmentorInterface.hpp>
#include <Logger/Logger.hpp>
#include <Logger/ActiveObjectCallback.hpp>
#include <Logger/StreamLogger.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <CORBACommons/ObjectPool.hpp>

#include <CampaignSvcs/CampaignServer/CampaignServer.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignSvcsVersionAdapter.hpp>
#include <CampaignSvcs/CampaignManager/DomainParser.hpp>
#include <xsd/Frontends/FeConfig.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserIdController.hpp>

namespace AdServer
{
  namespace Configuration
  {
    using namespace xsd::AdServer::Configuration;
  }

  class CommonModule:
    private Logging::LoggerCallbackHolder,
    private virtual Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    CommonModule(Logging::Logger* logger = 0) throw (eh::Exception);

    void
    init() throw();

    void
    shutdown() throw();

    UserIdController_var
    user_id_controller() const throw();

    FrontendCommons::UrlMatcher_var
    url_matcher() const throw();

    FrontendCommons::WebBrowserMatcher_var
    web_browser_matcher() const throw();

    FrontendCommons::PlatformMatcher_var
    platform_matcher() const throw();

    FrontendCommons::IPMatcher_var
    ip_matcher() const throw();

    FrontendCommons::CountryFilter_var
    country_filter() const throw();

    Language::Segmentor::SegmentorInterface_var
    segmentor() const throw();

    void
    update(unsigned service_index) throw();

    CampaignSvcs::DomainParser_var
    domain_parser() const throw ();

    AdServer::CampaignSvcs::ColocationFlagsSeq_var
    get_colocation_flags(unsigned service_index) throw (Exception);

    void set_config_file(const char* config_file) throw ();

  private:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;

    typedef CORBACommons::ObjectPoolRefConfiguration
      CampaignServerPoolConfig;

    typedef CORBACommons::ObjectPool<
      AdServer::CampaignSvcs::CampaignServer,
      CampaignServerPoolConfig>
      CampaignServerPool;

    typedef Configuration::CommonFeConfigurationType
      CommonFeConfiguration;
    typedef Configuration::DomainConfigurationType
      DomainConfiguration;

    typedef std::unique_ptr<CampaignServerPool> CampaignServerPoolPtr;

    typedef std::unique_ptr<CommonFeConfiguration> CommonConfigPtr;
    typedef std::unique_ptr<DomainConfiguration> DomainConfigPtr;

  protected:
    virtual
    ~CommonModule() throw();

  private:
    void parse_config_(
      CommonConfigPtr& common_config,
      DomainConfigPtr& domain_config)
      throw (Exception);

  private:
    /* configuration */
    std::string config_file_;

    Generics::TaskRunner_var task_runner_;
    Generics::Planner_var scheduler_;
    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;

    UserIdController_var user_id_controller_;

    CampaignServerPoolPtr campaign_servers_;
    mutable SyncPolicy::Mutex matchers_lock_;
    FrontendCommons::UrlMatcher_var url_matcher_;
    FrontendCommons::WebBrowserMatcher_var web_browser_matcher_;
    FrontendCommons::PlatformMatcher_var platform_matcher_;
    FrontendCommons::IPMatcher_var ip_matcher_;
    FrontendCommons::CountryFilter_var country_filter_;
    Generics::Time matchers_timestamp_;

    CampaignSvcs::DomainParser_var domain_parser_;
    Language::Segmentor::SegmentorInterface_var segmentor_;

  };

  typedef ReferenceCounting::SmartPtr<CommonModule> CommonModule_var;
}

/* Inlines */
namespace AdServer
{
  inline
  CommonModule::~CommonModule() throw ()
  {}

  inline
  void
  CommonModule::set_config_file(const char* config_file) throw ()
  {
    config_file_ = config_file;
  }

  inline
  UserIdController_var
  CommonModule::user_id_controller() const throw()
  {
    return user_id_controller_;
  }

  inline
  CampaignSvcs::DomainParser_var
  CommonModule::domain_parser() const throw ()
  {
    return domain_parser_;
  }
} // namespace AdServer
