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

#ifndef _REQUEST_INFO_FILLER_HPP_
#define _REQUEST_INFO_FILLER_HPP_

#include <string>

#include <GeoIP/IPMap.hpp>
#include <Logger/Logger.hpp>
#include <Generics/GnuHashTable.hpp>

#include <HTTP/Http.hpp>

#include <Commons/Containers.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserAgentMatcher.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>
#include <Frontends/FrontendCommons/FCGI.hpp>
#include <Frontends/CommonModule/CommonModule.hpp>
#include <Commons/LogReferrerUtils.hpp>

#include "RequestInfo.hpp"

namespace AdServer
{
  class DebugInfoStatus;

  typedef FrontendCommons::RequestParamProcessor<RequestInfo>
    RequestInfoParamProcessor;

  typedef ReferenceCounting::SmartPtr<RequestInfoParamProcessor>
    RequestInfoParamProcessor_var;

  class RequestInfoFiller: public FrontendCommons::HTTPExceptions
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    struct ColoFlags
    {
      unsigned long flags;
      bool hid_profile;
    };

    class ColoFlagsMap:
      public std::map<unsigned long, ColoFlags>,
      public ReferenceCounting::AtomicImpl
    {
    protected:
      virtual ~ColoFlagsMap() throw () {}
    };

    typedef ReferenceCounting::SmartPtr<ColoFlagsMap>
      ColoFlagsMap_var;

    RequestInfoFiller(
      Logging::Logger* logger,
      unsigned long colo_id,
      CommonModule* common_module,
      const char* geo_ip_path,
      const char* user_agent_filter_path,
      SetUidController* set_uid_controller,
      std::set<std::string>* ip_list,
      const std::set<int>& colo_list,
      Commons::LogReferrer::Setting log_referrer_site_stas_setting)
      throw(eh::Exception);

    void fill(RequestInfo& request_info,
      DebugInfoStatus* debug_info,
      const FCGI::HttpRequest& request) const
      throw(InvalidParamException, ForbiddenException, Exception);

    const Logging::Logger_var&
    logger() const throw ()
    {
      return logger_;
    }

    void colo_flags(ColoFlagsMap* colo_flags) throw();

    void adapt_client_id_(
      const String::SubString& in,
      RequestInfo& request_info,
      bool persistent,
      bool household,
      bool merged_persistent,
      bool rewrite_persistent) const
      throw(InvalidParamException);

    typedef Generics::GnuHashTable<
      Generics::SubStringHashAdapter, RequestInfoParamProcessor_var>
      ParamProcessorMap;

  private:
    typedef Sync::Policy::PosixThread SyncPolicy;

    typedef std::unique_ptr<GeoIPMapping::IPMapCity2> IPMapPtr;

  private:
    ColoFlagsMap_var
    get_colocations_() const throw();

    void sign_client_id_(
      std::string& signed_uid,
      const AdServer::Commons::UserId& uid) const
      throw();

    void add_processor_(
      bool headers,
      bool parameters,
      const String::SubString& name,
      const RequestInfoParamProcessor_var& processor)
      throw();

    void
    add_processors_(
      bool headers,
      bool parameters,
      const std::initializer_list<String::SubString>& names,
      const RequestInfoParamProcessor_var& processor)
      throw();

  private:
    Logging::Logger_var logger_;
    unsigned long colo_id_;
    std::set<std::string> ip_list_;
    std::set<int> colo_list_;
    bool use_ip_list_;
    Commons::LogReferrer::Setting log_referrer_setting_;

    CommonModule_var common_module_;
    IPMapPtr ip_map_;
    FrontendCommons::UserAgentMatcher user_agent_matcher_;
    SetUidController_var set_uid_controller_;

    mutable SyncPolicy::Mutex colocations_lock_;
    ColoFlagsMap_var colocations_;

    ParamProcessorMap header_processors_;
    ParamProcessorMap param_processors_;
    ParamProcessorMap cookie_processors_;
  };
}

namespace AdServer
{
  inline
  void
  RequestInfoFiller::colo_flags(ColoFlagsMap* colo_flags)
    throw()
  {
    ColoFlagsMap_var colo_flags_tmp = ReferenceCounting::add_ref(colo_flags);

    SyncPolicy::WriteGuard lock(colocations_lock_);
    colocations_.swap(colo_flags_tmp);
  }

  inline
  RequestInfoFiller::ColoFlagsMap_var
  RequestInfoFiller::get_colocations_() const
    throw()
  {
    SyncPolicy::ReadGuard lock(colocations_lock_);
    return colocations_;
  }
}

#endif /*_REQUEST_INFO_FILLER_HPP_*/
