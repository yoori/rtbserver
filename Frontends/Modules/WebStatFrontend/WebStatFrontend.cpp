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

#include <Generics/Time.hpp>
#include <Logger/StreamLogger.hpp>
#include <HTTP/HTTPCookie.hpp>
#include <HTTP/UrlAddress.hpp>
#include <String/StringManip.hpp>
#include <PrivacyFilter/Filter.hpp>
#include <Commons/ErrorHandler.hpp>
#include <Commons/CorbaAlgs.hpp>

#include "WebStatFrontend.hpp"

namespace AdServer
{
namespace WebStat
{
  namespace
  {
    struct FrontendConstrainTraits
    {
      static const unsigned long MAX_NUMBER_PARAMS = 50;
      static const unsigned long MAX_LENGTH_PARAM_NAME = 30;
      static const unsigned long MAX_LENGTH_PARAM_VALUE = 500;
    };

    typedef FrontendCommons::DefaultConstrain<
      FrontendCommons::OnlyGetAndPostAllowed,
      FrontendCommons::ParamConstrainDefault,
      FrontendConstrainTraits>
      FrontendHTTPConstrain;

    namespace Config
    {
      const char CONFIG_FILE[] = "WebStatFrontend_Config";
      const char ENABLE[] = "WebStatFrontend_Enable";
    }

    namespace Aspect
    {
      const char WEBSTAT_FRONTEND[] = "WebStatFrontend";
    }
  }

  /**
   * Frontend implementation
   */
  Frontend::Frontend(
    Configuration* frontend_config,
    Logging::Logger* logger,
    CommonModule* common_module)
    throw(eh::Exception)
    : Logging::LoggerCallbackHolder(
        Logging::Logger_var(
          new Logging::SeveritySelectorLogger(
            logger,
            0,
            frontend_config->get().WebStatFeConfiguration()->Logger().log_level())),
        "WebStat::Frontend",
        Aspect::WEBSTAT_FRONTEND,
        0),
      frontend_config_(ReferenceCounting::add_ref(frontend_config)),
      common_module_(ReferenceCounting::add_ref(common_module)),
      campaign_managers_(this->logger(), Aspect::WEBSTAT_FRONTEND)
  {}

  void
  Frontend::parse_config_() throw(Exception)
  {
    ::Config::ErrorHandler error_handler;

    const auto& fe_config = frontend_config_->get();

    if(!fe_config.CommonFeConfiguration().present())
    {
      throw Exception("CommonFeConfiguration isn't present");
    }

    common_config_ = CommonConfigPtr(
      new CommonFeConfiguration(*fe_config.CommonFeConfiguration()));

    if(!fe_config.WebStatFeConfiguration().present())
    {
      throw Exception("WebStatFeConfiguration isn't present");
    }

    config_ = ConfigPtr(
      new WebStatFeConfiguration(*fe_config.WebStatFeConfiguration()));
  }

  bool
  Frontend::will_handle(const String::SubString& uri) throw ()
  {
    std::string found_uri;
    bool result = FrontendCommons::find_uri(
      config_->UriList().Uri(), uri, found_uri);

    if(logger()->log_level() >= Logging::Logger::TRACE)
    {
      Stream::Error ostr;
      ostr << "WebStat::Frontend::will_handle(" << uri << "), result " << result;

      logger()->log(ostr.str(),
        Logging::Logger::TRACE,
        Aspect::WEBSTAT_FRONTEND);
    }

    return result;
  }

  int
  Frontend::handle_request(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response)
    throw ()
  {
    static const char* FUN = "WebStat::Frontend::handle_request()";

    int http_result = 0;

    try
    {
      //FrontendHTTPConstrain::apply(request);

      RequestInfo request_info;
      request_info_filler_->fill(request_info, request.headers(), request.params());

      CampaignSvcs::CampaignManager::WebOperationInfo web_op_info;
      //web_op_info.check_args = true;
      web_op_info.time = CorbaAlgs::pack_time(request_info.time);
      web_op_info.colo_id = request_info.colo_id;
      web_op_info.tag_id = request_info.tag_id;
      web_op_info.cc_id = request_info.cc_id;
      web_op_info.ct << request_info.ct;
      web_op_info.curct << request_info.curct;
      web_op_info.browser << request_info.browser;
      web_op_info.os << request_info.os;
      web_op_info.app << request_info.application;
      web_op_info.source << request_info.source;
      web_op_info.operation << request_info.operation;
      web_op_info.result = request_info.result;
      web_op_info.user_status = static_cast<unsigned long>(request_info.user_status);
      web_op_info.test_request = request_info.test_request;
      web_op_info.referer << request_info.referer;
      web_op_info.ip_address << request_info.peer_ip;
      web_op_info.external_user_id << request_info.external_user_id;
      web_op_info.user_agent << request_info.user_agent;
      if (!request_info.request_ids.empty())
      {
        web_op_info.request_ids.length(request_info.request_ids.size());
        CORBA::ULong ri = 0;
        for(RequestIdSet::const_iterator rit = request_info.request_ids.begin();
            rit != request_info.request_ids.end(); ++rit, ++ri)
        {
          if (!rit->is_null())
          {
            web_op_info.request_ids[ri] = CorbaAlgs::pack_request_id(*rit);
          }
        }
      }
      if (!request_info.global_request_id.is_null())
      {
        web_op_info.global_request_id =
          CorbaAlgs::pack_request_id(request_info.global_request_id);
      }

      campaign_managers_.consider_web_operation(web_op_info);

      if(!request_info.origin.empty())
      {
        response.add_header(
          String::SubString("Access-Control-Allow-Origin"),
          request_info.origin);

        response.add_header(
          String::SubString("Access-Control-Allow-Credentials"),
          String::SubString("true"));
      }

      // additional response headers
      if(common_config_->ResponseHeaders().present())
      {
        FrontendCommons::add_headers(
          *(common_config_->ResponseHeaders()),
          response);
      }

      if(request.method() == FCGI::HttpRequest::RM_GET)
      {
        response.set_content_type(String::SubString("image/gif"));

        FileCache::BufferHolder_var buffer = pixel_->get();
        response.get_output_stream().write((*buffer)->data(), (*buffer)->size());
      }
    }
    catch (const ForbiddenException& ex)
    {
      http_result = 403;
      logger()->sstream(Logging::Logger::TRACE, Aspect::WEBSTAT_FRONTEND) <<
        FUN << ": ForbiddenException caught: " << ex.what();
    }
    catch (const InvalidParamException& ex)
    {
      http_result = 400;
    }
    catch(const AdServer::CampaignSvcs::CampaignManager::IncorrectArgument&)
    {
      http_result = 400;
    }
    catch(const eh::Exception& e)
    {
      http_result = 500;
      logger()->sstream(
        Logging::Logger::EMERGENCY,
        Aspect::WEBSTAT_FRONTEND,
        "ADS-IMPL-139") << FUN << ": eh::Exception has been caught: " << e.what();
    }

    return http_result;
  }

  void
  Frontend::init() throw (eh::Exception)
  {
    static const char* FUN = "WebStat::Frontend::init()";

    if(true) // module_used())
    {
      try
      {
        parse_config_();

        corba_client_adapter_ = new CORBACommons::CorbaClientAdapter();

        pixel_ = FileCachePtr(
          new FileCache(config_->pixel_path().c_str()));

        campaign_managers_.resolve(
          *common_config_, corba_client_adapter_);

        request_info_filler_.reset(new RequestInfoFiller(
          config_->rid_public_key().c_str(),
          common_module_.in()));
      }
      catch (const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": eh::Exception caught: " << ex.what();
        throw Exception(ostr);
      }

      logger()->log(String::SubString(
          "WebStat::Frontend::init(): frontend is running ..."),
        Logging::Logger::INFO,
        Aspect::WEBSTAT_FRONTEND);
    }
  }

  void
  Frontend::shutdown() throw ()
  {
    logger()->log(String::SubString(
      "WebStat::Frontend::shutdown(): frontend terminated"),
      Logging::Logger::INFO,
      Aspect::WEBSTAT_FRONTEND);
  }
} /*WebStat*/
} /*AdServer*/
