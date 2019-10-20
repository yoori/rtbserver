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
#include <Commons/ErrorHandler.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>
#include <Frontends/FrontendCommons/ProcessingTemplates.hpp>

#include "NullAdFrontend.hpp"

namespace Request
{
  namespace Headers
  {
    const String::AsciiStringManip::Caseless SECURE("secure");
  }

  namespace Parameters
  {
    const String::SubString ORIG("orig");
    const String::SubString TAG_ID("tid");
    const String::SubString RANDOM("random");
    const String::SubString APP_FORMAT("format");
    const String::SubString DATA("d");
    const String::SubString PASSBACK_TYPE("pt");
    const String::SubString OLD_PASSBACK_URL("passback");
    const String::SubString PASSBACK_URL("pb");
  }
}

namespace
{
  template<typename Type>
  std::string
  to_string(const Type& i)
  {
    std::ostringstream ostr;
    ostr << i;
    return ostr.str();
  }
}

namespace AdServer
{
  struct NullFrontendParamConstraint : FrontendCommons::HTTPExceptions
  {
    template <typename ConstrainTraits>
    static void
    apply(const HTTP::SubParam& param) throw (InvalidParamException)
    {
      static const char* FUN = "AdFrontendParamConstrain::apply()";

      if (param.name.size() > ConstrainTraits::MAX_LENGTH_PARAM_NAME)
      {
        Stream::Error ostr;
        ostr << FUN << ": Param name length(" << param.name.size() <<
          ") exceed";

        throw InvalidParamException(ostr);
      }

      if (param.value.size() > ConstrainTraits::MAX_LENGTH_PARAM_VALUE)
      {
        if (param.name != Request::Parameters::DATA)
        {
          Stream::Error ostr;
          ostr << FUN << ": Param value length(" << param.value.size() <<
            ") exceed";

          throw InvalidParamException(ostr);
        }
      }
    }
  };
}

namespace
{
  const String::AsciiStringManip::Caseless URL_LOCAL_PREFIX("//");

  struct NullAdFrontendConstrainTraits
  {
    static const unsigned long MAX_NUMBER_PARAMS = 50;
    static const unsigned long MAX_LENGTH_PARAM_NAME = 30;
    static const unsigned long MAX_LENGTH_PARAM_VALUE = 2000;
  };

  typedef FrontendCommons::DefaultConstrain<
    FrontendCommons::OnlyGetAndPostAllowed,
    AdServer::NullFrontendParamConstraint,
    NullAdFrontendConstrainTraits>
      NullAdFrontendHTTPConstrain;

  const char HANDLE_COMMAND_ERROR[] =
    "NullAdFrontend::handle_command: an error occurred";
}

namespace Config
{
  const char ENABLE[] = "NullAdFrontend_Enable";
  const char CONFIG_FILE[] = "NullAdFrontend_ConfigFile";
}

namespace Aspect
{
  const char NULL_AD_FRONTEND[] = "NullAdFrontend";
}

namespace AdServer
{
  /**
   * PassbackFrontend implementation
   */
  NullAdFrontend::NullAdFrontend(
    Configuration* frontend_config,
    Logging::Logger* logger)
    throw (eh::Exception)
    : Logging::LoggerCallbackHolder(
        logger,
        0,
        Aspect::NULL_AD_FRONTEND,
        0),
      frontend_config_(ReferenceCounting::add_ref(frontend_config))
  {}

  bool
  NullAdFrontend::will_handle(const String::SubString& uri)
    throw()
  {
    std::string found_uri;
    bool result = FrontendCommons::find_uri(
      config_->UriList().Uri(), uri, found_uri);

    if(logger()->log_level() >= TraceLevel::MIDDLE)
    {
      Stream::Error ostr;
      ostr << "NullAdFrontend::will_handle(" << uri << "), result " <<
        result;

      logger()->log(ostr.str(), TraceLevel::MIDDLE, Aspect::NULL_AD_FRONTEND);
    }

    return result;
  }

  int
  NullAdFrontend::handle_request(
    const FCGI::HttpRequest& request,
    FCGI::HttpResponse& response) throw ()
  {
    static const char* FUN = "NullAdFrontend::handle_request";

    int http_status = 204;

    try
    {
      NullAdFrontendHTTPConstrain::apply(request);

      RequestInfo info;

      for(HTTP::ParamList::const_iterator it =
            request.params().begin();
          it != request.params().end(); ++it)
      {
        parse_param_(it->name, it->value, info);
      }

      const HTTP::SubHeaderList& headers = request.headers();
      for (HTTP::SubHeaderList::const_iterator it = headers.begin();
        it != headers.end(); ++it)
      {
        if(it->name == Request::Headers::SECURE)
        {
          if(it->value.size() == 1 && it->value.at(0) == '1')
          {
            info.secure = true;
          }
          break;
        }
      }

      if((info.passback_type != "html" &&
          info.passback_type != "js" &&
          info.passback_type != "redir") ||
         info.passback_url.empty())
      {
        info.passback_type.clear();
      }

      bool request_defined_passback_url = !info.passback_url.empty();

      Generics::SubStringHashAdapter instantiate_type =
        FrontendCommons::deduce_instantiate_type(&info.secure, request);

      if(info.tag_id == 0)
      {
        return 204;
      }

      if(info.passback_type == "redir")
      {
        http_status = FrontendCommons::redirect(
          info.passback_url,
          response);
      }
      else
      {
        std::string mime_format;

        // get tag passback url from CampaignServer
        try
        {
          for (;;)
          {
            CampaignServerPool::ObjectHandlerType campaign_server =
              campaign_servers_->get_object<CampaignServerPool::Exception>(
                logger(),
                Logging::Logger::EMERGENCY,
                Aspect::NULL_AD_FRONTEND,
                "ADS-ICON-4",
                common_config_->service_index(),
                common_config_->service_index());

            try
            {
              AdServer::CampaignSvcs::CampaignServer::PassbackInfo_var
                tag_passback = campaign_server->get_tag_passback(
                  info.tag_id, info.request_app_format.c_str());

              if (!tag_passback->active)
              {
                return 204;
              }
              bool init_passback_url = false;

              if(info.passback_type.empty())
              {
                init_passback_url = true;
                info.passback_url = tag_passback->passback_url.in();
                info.passback_type = tag_passback->passback_type.in();
              }

              info.width = tag_passback->width;
              info.height = tag_passback->height;
              info.size = tag_passback->size;
              info.passback_code = tag_passback->passback_code;

              mime_format = tag_passback->mime_format.in();

              if(init_passback_url && !info.passback_url.empty())
              {
                if(!HTTP::HTTP_PREFIX.start(info.passback_url) &&
                   !HTTP::HTTPS_PREFIX.start(info.passback_url) &&
                   !URL_LOCAL_PREFIX.start(info.passback_url))
                {
                  if(!info.secure)
                  {
                    info.passback_url =
                      config_->local_passback_prefix() + info.passback_url;
                  }
                  else
                  {
                    info.passback_url =
                      config_->secure_local_passback_prefix() + info.passback_url;
                  }
                }
              }
              break;
            }
            catch(const AdServer::CampaignSvcs::CampaignServer::NotReady& exc)
            {
              logger()->sstream(Logging::Logger::NOTICE,
                Aspect::NULL_AD_FRONTEND,
                "ADS-ICON-4") <<
                FUN << ": CampaignServer::NotReady caught: " <<
                exc.description;
              campaign_server.release_bad(
                String::SubString("CampaignServer is not ready"));
            }
            catch(const AdServer::CampaignSvcs::
              CampaignServer::ImplementationException& e)
            {
              Stream::Error ostr;
              ostr << FUN << ": caught ImplementationException: " <<
                e.description;
              campaign_server.release_bad(ostr.str());
              logger()->log(ostr.str(),
                Logging::Logger::EMERGENCY,
                Aspect::NULL_AD_FRONTEND,
                "ADS-IMPL-193");
            }
            catch(const CORBA::SystemException& e)
            {
              Stream::Error ostr;
              ostr << FUN << ": caught CORBA::SystemException: " << e;
              campaign_server.release_bad(ostr.str());
              logger()->log(ostr.str(),
                Logging::Logger::EMERGENCY,
                Aspect::NULL_AD_FRONTEND,
                "ADS-ICON-4");
            }
          }
        }
        catch(const CampaignServerPool::Exception& ex)
        {
          logger()->sstream(Logging::Logger::ERROR, Aspect::NULL_AD_FRONTEND) <<
            FUN << ": caught CampaignServerPool::Exception: " <<
            ex.what();
        }

        if(!mime_format.empty())
        {
          if(!info.passback_url.empty() &&
             !URL_LOCAL_PREFIX.start(info.passback_url))
          {
            HTTP::HTTPAddress addr(info.passback_url);
            info.passback_url = addr.url();
          }
        }

        if(common_config_->ResponseHeaders().present())
        {
          FrontendCommons::add_headers(
            *(common_config_->ResponseHeaders()),
            response);
        }

        try
        {
          TemplateRuleMap::const_iterator rule_it =
            template_rules_.find(instantiate_type);

          if(rule_it != template_rules_.end())
          {
            Generics::StringHashAdapter file_path =
              config_->passback_template_path_prefix() + info.request_app_format;

            AdServer::CampaignSvcs::PassbackTemplate_var passback_template =
              passback_templates_.get(file_path);

            AdServer::CampaignSvcs::TokenValueMap request_args;

            for(TokenValueMap::const_iterator it = rule_it->second.tokens.begin();
                it != rule_it->second.tokens.end(); ++it)
            {
              request_args[it->first] = it->second;
            }

            request_args["TAGWIDTH"] = to_string(info.width);
            request_args["TAGHEIGHT"] = to_string(info.height);
            request_args["TAGSIZE"] = info.size;
            request_args["TAGID"] = to_string(info.tag_id);
            request_args["PASSBACK_TYPE"] = info.passback_type;
            request_args["PASSBACK_URL"] = info.passback_url;
            request_args["PASSBACK_CODE"] =
              !request_defined_passback_url ? info.passback_code : std::string();
            request_args["RANDOM"] = to_string(info.random);

            std::ostringstream passback_body_ostr;

            passback_template->instantiate(
              passback_body_ostr,
              request_args);

            response.set_content_type(mime_format);
            response.get_output_stream().write(
              passback_body_ostr.str().data(),
              passback_body_ostr.str().size());

            http_status = 200;
          }
        }
        catch(const eh::Exception& e)
        {
          Stream::Error ostr;
          ostr << FUN <<
            ": Caught eh::Exception on instantiate passback template for: '" <<
            info.passback_url << "': " << e.what();
          logger()->log(ostr.str(), Logging::Logger::ERROR,
            Aspect::NULL_AD_FRONTEND, "ADS-IMPL-194");
        }
      }
    }
    catch (const NullAdFrontendHTTPConstrain::ForbiddenException& ex)
    {
      http_status = 403;
      logger()->sstream(TraceLevel::LOW, Aspect::NULL_AD_FRONTEND)
        << FUN << ": ForbiddenException caught: " << ex.what();
    }
    catch (const FrontendCommons::HTTPExceptions::InvalidParamException& ex)
    {
      http_status = 400;
      logger()->sstream(TraceLevel::MIDDLE, Aspect::NULL_AD_FRONTEND)
        << FUN << ": InvalidParamException caught: " << ex.what();
    }
    catch(const eh::Exception& e)
    {
      http_status = 500;
      Stream::Error ostr;
      ostr << FUN << ": eh::Exception caught: " << e.what();
      logger()->log(ostr.str(), Logging::Logger::EMERGENCY,
        Aspect::NULL_AD_FRONTEND, "ADS-IMPL-191");
    }

    return http_status;
  }

  void
  NullAdFrontend::init() throw (eh::Exception)
  {
    static const char* FUN = "NullAdFrontend::init()";

    if(true) // module_used())
    {
      try
      {
        corba_client_adapter_ = new CORBACommons::CorbaClientAdapter();

        campaign_servers_ = resolve_campaign_servers_();

        for(auto rule_it = common_config_->TemplateRule().begin();
          rule_it != common_config_->TemplateRule().end();
          ++rule_it)
        {
          strings_.push_back(rule_it->name());

          TemplateRule& template_rule = template_rules_[
            String::SubString(strings_.back())];

          for(auto token_it = rule_it->XsltToken().begin();
            token_it != rule_it->XsltToken().end();
            ++token_it)
          {
            template_rule.tokens[token_it->name()] = token_it->value();
          }
        }
      }
      catch (const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << FUN << ": eh::Exception caught: " << ex.what();
        throw Exception(ostr);
      }
    }
  }

  void
  NullAdFrontend::shutdown() throw ()
  {
    logger()->log(String::SubString(
        "NullAdFrontend::shutdown: frontend terminated"),
      Logging::Logger::INFO, Aspect::NULL_AD_FRONTEND);
  }

  NullAdFrontend::CampaignServerPoolPtr
  NullAdFrontend::resolve_campaign_servers_()
    throw(Exception, eh::Exception)
  {
    static const char* FUN = "NullAdFrontend::resolve_campaign_servers_()";

    try
    {
      CORBACommons::CorbaObjectRefList campaign_server_refs;

      Config::CorbaConfigReader::read_multi_corba_ref(
        common_config_->CampaignServerCorbaRef(),
        campaign_server_refs);

      CampaignServerPoolConfig pool_config(corba_client_adapter_.in());
      pool_config.timeout = Generics::Time(10); // 10 sec

      std::copy(
        campaign_server_refs.begin(),
        campaign_server_refs.end(),
        std::back_inserter(pool_config.iors_list));

      return CampaignServerPoolPtr(
        new CampaignServerPool(
          pool_config, CORBACommons::ChoosePolicyType::PT_PERSISTENT));
    }
    catch (const CORBA::SystemException& e)
    {
      Stream::Error ostr;
      ostr << FUN << ": got CORBA::SystemException: " << e;
      throw Exception(ostr);
    }
  }

  void NullAdFrontend::parse_param_(
    const String::SubString& name,
    const String::SubString& value,
    RequestInfo& info)
    throw(FrontendCommons::HTTPExceptions::InvalidParamException)
  {
    String::AsciiStringManip::Caseless option_name(name);
    if(option_name == Request::Parameters::PASSBACK_URL ||
       option_name == Request::Parameters::OLD_PASSBACK_URL)
    {
      try
      {
        HTTP::BrowserAddress addr(value);
        addr.get_view(
          HTTP::HTTPAddress::VW_FULL,
          info.passback_url);
      }
      catch(...)
      {}
    }
    else if(option_name == Request::Parameters::PASSBACK_TYPE)
    {
      value.assign_to(info.passback_type);
    }
    else if(option_name == Request::Parameters::ORIG)
    {
      if(info.passback_url.empty())
      {
        try
        {
          HTTP::BrowserAddress addr(value);
          addr.get_view(
            HTTP::HTTPAddress::VW_FULL,
            info.passback_url);
        }
        catch(...)
        {}
      }
    }
    else if(option_name == Request::Parameters::TAG_ID)
    {
      unsigned long tag_id_val;
      if(String::StringManip::str_to_int(value, tag_id_val))
      {
        info.tag_id = tag_id_val;
      }
    }
    else if(option_name == Request::Parameters::RANDOM)
    {
      unsigned long random_val;
      if(String::StringManip::str_to_int(value, random_val))
      {
        if(random_val > 9999999)
        {
          Stream::Error err;
          err << "random value = " << random_val << " is too big";
          throw FrontendCommons::HTTPExceptions::InvalidParamException(err);
        }
        info.random = random_val;
      }
    }
    else if(option_name == Request::Parameters::APP_FORMAT)
    {
      value.assign_to(info.request_app_format);
    }
    else if(option_name == Request::Parameters::DATA)
    {
      std::string decoded;
      try
      {
        String::StringManip::base64mod_decode(decoded, value);
      }
      catch(const String::StringManip::InvalidFormatException&)
      {
        throw FrontendCommons::HTTPExceptions::InvalidParamException("");
      }
      static const char AMP = '&';
      static const char EQ = '=';
      std::string::const_iterator start_name = decoded.begin();
      std::string::const_iterator end_name = decoded.begin();
      bool replace_double_amp = false, found_eq = false;
      for(std::string::const_iterator it = decoded.begin();
          it <= decoded.end(); ++it)
      {
        if(it == decoded.end() || *it == AMP)
        {
          if(it + 1 < decoded.end() && *(it + 1) == AMP)
          {
            replace_double_amp = true;
            ++it;// skip next amp
          }
          else
          {
            std::string purify_value;
            HTTP::SubParam param(
              String::SubString(&*start_name, &*end_name),
              String::SubString(&*(end_name + 1), it - end_name - 1));
            if(replace_double_amp)
            {
              String::StringManip::replace(
                param.value,
                purify_value,
                String::SubString("&&", 2),
                String::SubString("&", 1)); 
              param.value = purify_value;
            }
            parse_param_(param.name, param.value, info);
            replace_double_amp = false;
            found_eq = false;
            start_name = it + 1;
            end_name = start_name;
          }
        }
        else if(!found_eq && *it == EQ)
        {
          end_name = it;
          found_eq = true;
        }
      }
    }
  }

}
