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

#ifndef FRONTENDS_WEBSTATFRONTEND_REQUESTINFOFILLER_HPP
#define FRONTENDS_WEBSTATFRONTEND_REQUESTINFOFILLER_HPP

#include <string>
#include <map>
#include <vector>

#include <GeoIP/IPMap.hpp>
#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/Uuid.hpp>
#include <Generics/GnuHashTable.hpp>

#include <HTTP/Http.hpp>

#include <Language/SegmentorCommons/SegmentorInterface.hpp>

#include <Commons/Containers.hpp>
#include <Commons/UserInfoManip.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserAgentMatcher.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>
#include <Frontends/CommonModule/CommonModule.hpp>

namespace AdServer
{
namespace WebStat
{
  typedef std::set<AdServer::Commons::RequestId> RequestIdSet;

  struct RequestInfo
  {
    RequestInfo()
      : time(Generics::Time::ZERO),
        colo_id(0),
        tag_id(0),
        cc_id(0),
        result('U'),
        user_status(AdServer::CampaignSvcs::US_UNDEFINED),
        test_request(false)
    {}

    Generics::Time time;
    unsigned long colo_id;

    unsigned long tag_id;
    unsigned long cc_id;
    std::string ct;
    std::string curct;
    std::string browser;
    std::string os;
    std::string application;
    std::string source;
    std::string operation;
    char result;
    AdServer::CampaignSvcs::UserStatus user_status;
    bool test_request;
    std::string user_agent;
    std::string origin;
    RequestIdSet request_ids;
    AdServer::Commons::RequestId global_request_id;
    std::string referer;
    std::string peer_ip;
    std::string external_user_id;
  };

  typedef FrontendCommons::RequestParamProcessor<RequestInfo>
    RequestInfoParamProcessor;

  typedef ReferenceCounting::SmartPtr<RequestInfoParamProcessor>
    RequestInfoParamProcessor_var;

  class RequestInfoFiller: public FrontendCommons::HTTPExceptions
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    RequestInfoFiller(
      const char* public_key,
      CommonModule* common_module)
      throw (eh::Exception);

    void fill(RequestInfo& request_info,
      const HTTP::SubHeaderList& headers,
      const HTTP::ParamList& params) const
      throw(InvalidParamException, ForbiddenException, Exception);

  private:
    typedef Sync::Policy::PosixThread SyncPolicy;
    
    typedef Generics::GnuHashTable<
      Generics::SubStringHashAdapter, RequestInfoParamProcessor_var>
      ParamProcessorMap;

    typedef std::unique_ptr<GeoIPMapping::IPMapCity2> IPMapPtr;

    typedef std::unique_ptr<Generics::SignedUuidVerifier> RequestIdVerifierPtr;

  private:
    void add_processor_(
      bool headers,
      bool parameters,
      const String::SubString& name,
      RequestInfoParamProcessor* processor)
      throw();

  private:
    const CommonModule_var common_module_;

    RequestIdVerifierPtr request_id_verifier_;

    ParamProcessorMap header_processors_;
    ParamProcessorMap param_processors_;
    ParamProcessorMap cookie_processors_;
  };
} /*WebStat*/
} /*AdServer*/

#endif /*FRONTENDS_WEBSTATFRONTEND_REQUESTINFOFILLER_HPP*/
