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

#ifndef FRONTENDS_PUBPIXELFRONTEND_REQUESTINFOFILLER_HPP
#define FRONTENDS_PUBPIXELFRONTEND_REQUESTINFOFILLER_HPP

#include <string>
#include <map>
#include <vector>

#include <Sync/SyncPolicy.hpp>
#include <GeoIP/IPMap.hpp>
#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/Uuid.hpp>
#include <Generics/GnuHashTable.hpp>

#include <HTTP/Http.hpp>

#include <Commons/Containers.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserAgentMatcher.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>

namespace AdServer
{
namespace PubPixel
{
  struct RequestInfo
  {
    RequestInfo()
      : user_status(AdServer::CampaignSvcs::US_UNDEFINED)
    {};

    AdServer::Commons::Optional<std::string> country;
    AdServer::CampaignSvcs::UserStatus user_status;
    std::string peer_ip;
    typedef std::vector<unsigned long> PublisherAccountIDs;
    PublisherAccountIDs publisher_account_ids;
  };

  typedef FrontendCommons::RequestParamProcessor<RequestInfo>
    RequestParamProcessor;

  typedef ReferenceCounting::SmartPtr<RequestParamProcessor>
    RequestParamProcessor_var;

  class RequestInfoFiller: public FrontendCommons::HTTPExceptions
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    RequestInfoFiller(
      Logging::Logger* logger,
      const char* geo_ip_path)
      throw(eh::Exception);

    void fill(RequestInfo& request_info,
      const HTTP::SubHeaderList& headers,
      const HTTP::ParamList& params) const
      throw(InvalidParamException, ForbiddenException, Exception);

    const Logging::Logger_var&
    logger() const throw ()
    {
      return logger_;
    }

  private:
    typedef Sync::Policy::PosixThread SyncPolicy;
    
    typedef Generics::GnuHashTable<
      Generics::SubStringHashAdapter, RequestParamProcessor_var>
      RequestParamProcessorMap;

    typedef std::unique_ptr<GeoIPMapping::IPMapCity2> IPMapPtr;

  private:
    Logging::Logger_var logger_;

    IPMapPtr ip_map_;

    RequestParamProcessorMap param_processors_;
    RequestParamProcessorMap header_processors_;
  };
} /*PubPixel*/
} /*AdServer*/

#endif /*FRONTENDS_PUBPIXELFRONTEND_REQUESTINFOFILLER_HPP*/
