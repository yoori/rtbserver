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
#include <Commons/ErrorHandler.hpp>
#include <Commons/CorbaAlgs.hpp>

#include "PassPixelFrontend.hpp"

namespace
{
  struct PassPixelFrontendConstrainTraits
  {
    static const unsigned long MAX_NUMBER_PARAMS = 50;
    static const unsigned long MAX_LENGTH_PARAM_NAME = 30;
    static const unsigned long MAX_LENGTH_PARAM_VALUE = 2000;
  };

  typedef FrontendCommons::DefaultConstrain<
    FrontendCommons::OnlyGetAllowed,
    FrontendCommons::ParamConstrainDefault,
    PassPixelFrontendConstrainTraits>
    PassPixelFrontendHTTPConstrain;
}

namespace Config
{
  const char CONFIG_FILE[] = "PassPixelFrontend_Config";
  const char ENABLE[] = "PassPixelFrontend_Enable";
}

namespace Aspect
{
  const char PASS_PIXEL_FRONTEND[] = "PassbackPixelFrontend";
}

namespace AdServer
{
namespace PassbackPixel
{
  /**
   * PassbackPixel::Frontend implementation
   */
  Frontend::Frontend(
    Configuration* frontend_config,
    Logging::Logger* logger,
    CommonModule* common_module)
    throw (eh::Exception)
    : Logging::LoggerCallbackHolder(
        Logging::Logger_var(
          new Logging::SeveritySelectorLogger(
            logger,
            0,
            frontend_config->get().PassPixelFeConfiguration()->Logger().log_level())),
        0,
        Aspect::PASS_PIXEL_FRONTEND,
        0),
      frontend_config_(ReferenceCounting::add_ref(frontend_config)),
      common_module_(ReferenceCounting::add_ref(common_module)),
      campaign_managers_(this->logger(), Aspect::PASS_PIXEL_FRONTEND)
  {}

  void
  Frontend::parse_config_() throw(Exception)
  {
    static const char* FUN = "Frontend::parse_config_()";

    try
    {
      typedef Configuration::FeConfig Config;
      const Config& fe_config = frontend_config_->get();

      if(!fe_config.CommonFeConfiguration().present())
      {
        throw Exception("CommonFeConfiguration isn't present");
      }

      common_config_ = CommonConfigPtr(
        new CommonFeConfiguration(*fe_config.CommonFeConfiguration()));

      if(!fe_config.PassPixelFeConfiguration().present())
      {
        throw Exception("PassPixelFeConfiguration isn't present");
      }

      config_ = ConfigPtr(new PassPixelFeConfiguration(
        *fe_config.PassPixelFeConfiguration()));

      request_info_filler_.reset(new RequestInfoFiller(
        logger(),
        common_config_->colo_id(),
        common_config_->GeoIP().present() ? common_config_->GeoIP()->path().c_str() : 0));

      track_pixel_ = FileCachePtr(
        new FileCache(config_->track_pixel_path().c_str()));
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": Can't parse config file: " << e.what();
      throw Exception(ostr);
    }
  }

  bool
  Frontend::will_handle(const String::SubString& uri) throw ()
  {
    std::string found_uri;

    bool result = FrontendCommons::find_uri(
      config_->UriList().Uri(), uri, found_uri);

    if(logger()->log_level() >= TraceLevel::MIDDLE)
    {
      Stream::Error ostr;
      ostr << "Frontend::will_handle(" << uri << "), result " << result;

      logger()->log(ostr.str(), TraceLevel::MIDDLE, Aspect::PASS_PIXEL_FRONTEND);
    }

    return result;
  }

  int
  Frontend::handle_request(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response) throw ()
  {
    static const char* FUN = "Frontend::handle_request()";

    try
    {
      // Checking requests validity
      PassPixelFrontendHTTPConstrain::apply(request);

      return handle_track_request(request, response);
    }
    catch (const ForbiddenException& ex)
    {
      logger()->sstream(TraceLevel::LOW, Aspect::PASS_PIXEL_FRONTEND) <<
        FUN << ": ForbiddenException caught: " << ex.what();

      return 403;
    }
    catch (const InvalidParamException& ex)
    {
      logger()->sstream(TraceLevel::MIDDLE, Aspect::PASS_PIXEL_FRONTEND) <<
        FUN << ": InvalidParamException caught: " << ex.what();

      return 400;
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": eh::Exception has been caught: " << e.what();
      logger()->log(ostr.str(), Logging::Logger::EMERGENCY,
        Aspect::PASS_PIXEL_FRONTEND, "ADS-IMPL-191");

      return 500;
    }
  }

  int
  Frontend::handle_track_request(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response)
    throw(ForbiddenException, InvalidParamException, eh::Exception)
  {
    static const char* FUN = "Frontend::handle_track_request()";

    int http_status = 200;

    PassbackTrackInfo passback_track_info;

    request_info_filler_->fill_track(
      passback_track_info,
      request.headers(),
      request.params());

    if(!passback_track_info.tag_id)
    {
      Stream::Error ostr;
      ostr << "Not correct tag_id";
      throw InvalidParamException(ostr);
    }

    try
    {
      if(common_config_->ResponseHeaders().present())
      {
        FrontendCommons::add_headers(
          *(common_config_->ResponseHeaders()),
          response);
      }

      response.set_content_type(String::SubString("image/gif"));

      FileCache::BufferHolder_var buffer = track_pixel_->get();
      response.get_output_stream().write((*buffer)->data(), (*buffer)->size());
    }
    catch(const eh::Exception& e)
    {
      http_status = 500;
      Stream::Error ostr;
      ostr << FUN << ": Caught eh::Exception: " << e.what();
      logger()->log(ostr.str(), Logging::Logger::ERROR,
        Aspect::PASS_PIXEL_FRONTEND, "ADS-IMPL-194");
    }

    AdServer::CampaignSvcs::CampaignManager::PassbackTrackInfo info;
    info.time = CorbaAlgs::pack_time(passback_track_info.time);
    info.country << passback_track_info.country;
    info.colo_id = passback_track_info.colo_id;
    info.tag_id = passback_track_info.tag_id;
    info.user_status = passback_track_info.user_status;
    campaign_managers_.consider_passback_track(info);

    return http_status;
  }

  void
  Frontend::init() throw (eh::Exception)
  {
    static const char* FUN = "Frontend::init()";

    if(true) // module_used())
    {
      try
      {
        parse_config_();

        corba_client_adapter_ = new CORBACommons::CorbaClientAdapter();

        campaign_managers_.resolve(
          *common_config_, corba_client_adapter_);
      }
      catch(const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": eh::Exception caught: " << ex.what();
        throw Exception(ostr);
      }

      logger()->log(String::SubString(
        "Frontend::init(): frontend is running ..."),
      Logging::Logger::INFO, Aspect::PASS_PIXEL_FRONTEND);
    }
  }

  void
  Frontend::shutdown() throw ()
  {
    logger()->log(String::SubString(
        "Frontend::shutdown(): frontend terminated"),
      Logging::Logger::INFO, Aspect::PASS_PIXEL_FRONTEND);
  }
} /*PassbackPixel*/
} /*AdServer*/
