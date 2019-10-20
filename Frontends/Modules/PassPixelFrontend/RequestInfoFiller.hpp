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

#ifndef FRONTENDS_PASSPIXELFRONTEND_REQUESTINFOFILLER_HPP
#define FRONTENDS_PASSPIXELFRONTEND_REQUESTINFOFILLER_HPP

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

#include <Commons/UserInfoManip.hpp>
#include <Commons/Containers.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserAgentMatcher.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>

namespace AdServer
{
namespace PassbackPixel
{
  struct PassbackTrackInfo
  {
    PassbackTrackInfo()
      : time(Generics::Time::ZERO),
        colo_id(0),
        tag_id(0),
        user_status(AdServer::CampaignSvcs::US_UNDEFINED)
    {};

    Generics::Time time;
    std::string country;
    unsigned long colo_id;
    unsigned long tag_id;
    AdServer::CampaignSvcs::UserStatus user_status;
    std::string ip;
  };

  typedef FrontendCommons::RequestParamProcessor<PassbackTrackInfo>
    PassbackTrackParamProcessor;

  typedef ReferenceCounting::SmartPtr<PassbackTrackParamProcessor>
    PassbackTrackParamProcessor_var;

  class RequestInfoFiller: public FrontendCommons::HTTPExceptions
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    RequestInfoFiller(
      Logging::Logger* logger,
      unsigned long colo_id,
      const char* geo_ip_path)
      throw(eh::Exception);

    void fill_track(
      PassbackTrackInfo& passback_track_info,
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
      Generics::SubStringHashAdapter, PassbackTrackParamProcessor_var>
      PassbackTrackProcessorMap;

    typedef std::unique_ptr<GeoIPMapping::IPMapCity2> IPMapPtr;

  private:
    Logging::Logger_var logger_;
    unsigned long colo_id_;

    IPMapPtr ip_map_;

    PassbackTrackProcessorMap track_header_processors_;
    PassbackTrackProcessorMap track_param_processors_;
    PassbackTrackProcessorMap track_cookie_processors_;
  };
} /*Passback*/
} /*AdServer*/

#endif /*FRONTENDS_PASSPIXELFRONTEND_REQUESTINFOFILLER_HPP*/
