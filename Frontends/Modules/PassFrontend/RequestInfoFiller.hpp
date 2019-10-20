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

#ifndef FRONTENDS_PASSBACKFRONTEND_REQUESTINFOFILLER_HPP
#define FRONTENDS_PASSBACKFRONTEND_REQUESTINFOFILLER_HPP

#include <string>
#include <map>
#include <vector>

#include <Sync/SyncPolicy.hpp>
#include <Logger/Logger.hpp>
#include <Generics/Time.hpp>
#include <Generics/Uuid.hpp>
#include <Generics/GnuHashTable.hpp>
#include <String/TextTemplate.hpp>

#include <HTTP/Http.hpp>

#include <Commons/UserInfoManip.hpp>
#include <Commons/Containers.hpp>
#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>
#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/RequestMatchers.hpp>
#include <Frontends/FrontendCommons/UserAgentMatcher.hpp>
#include <Frontends/FrontendCommons/UserInfoClient.hpp>
#include <Frontends/FrontendCommons/RequestParamProcessor.hpp>
#include <Frontends/CommonModule/CommonModule.hpp>

namespace AdServer
{
namespace Passback
{
  struct PassbackInfo
  {
    typedef std::list<unsigned long> AccountIdList;

    PassbackInfo()
      : time(Generics::Time::ZERO),
        test_request(false)
    {};

    AdServer::Commons::RequestId request_id;
    std::string passback_url;
    Generics::Time time;
    AdServer::Commons::Optional<unsigned long> user_id_hash_mod;
    bool test_request;
    AccountIdList pubpixel_accounts;
    AdServer::Commons::UserId current_user_id;

    std::string passback_url_templ;
    String::TextTemplate::Args tokens;
  };

  typedef FrontendCommons::RequestParamProcessor<PassbackInfo>
    PassbackParamProcessor;

  typedef ReferenceCounting::SmartPtr<PassbackParamProcessor>
    PassbackParamProcessor_var;

  class RequestInfoFiller: public FrontendCommons::HTTPExceptions
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    RequestInfoFiller(
      Logging::Logger* logger,
      CommonModule* common_module)
      throw(eh::Exception);

    void
    fill(
      PassbackInfo& passback_info,
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
      Generics::SubStringHashAdapter, PassbackParamProcessor_var>
      PassbackProcessorMap;

  private:
    Logging::Logger_var logger_;

    PassbackProcessorMap param_processors_;
    PassbackProcessorMap cookie_processors_;

  private:
    void
    cookies_processing_(
      PassbackInfo& passback_info,
      const HTTP::SubHeaderList& headers) const
      throw(InvalidParamException, Exception);
  };
} /*Passback*/
} /*AdServer*/

#endif /*FRONTENDS_PASSBACKFRONTEND_REQUESTINFOFILLER_HPP*/
