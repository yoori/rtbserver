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

#include <String/AsciiStringManip.hpp>
#include <Commons/ErrorHandler.hpp>

#include "FrontendInterface.hpp"

namespace FrontendCommons
{
  namespace
  {
    const String::AsciiStringManip::CharCategory NONTOKEN(
      "\x01-\x31()<>@,;:\\\"/[]?={} \t", true);

    const String::AsciiStringManip::Caseless FORMURL(
      "application/x-www-form-urlencoded");
  }

  // FrontendInterface::Configuration
    
  FrontendInterface::Configuration::Configuration(
    const char* config_path) :
    config_path_(config_path)
  { }
  
  void
  FrontendInterface::Configuration::read()
    throw(InvalidConfiguration)
  {
    Config::ErrorHandler error_handler;
    
    try
    {
      config_ =
        xsd::AdServer::Configuration::FeConfiguration(
          config_path_.c_str(), error_handler);
    
      if(error_handler.has_errors())
      {
        std::string error_string;
        throw InvalidConfiguration(error_handler.text(error_string));
      }
    }
    catch (const xml_schema::parsing& e)
    {
      Stream::Error err;
      err << "Can't parse config file '"
          << config_path_ << "'."
          << ": ";
      if(error_handler.has_errors())
      {
        std::string error_string;
        err << error_handler.text(error_string);
      }
      throw InvalidConfiguration(err);
    }
  }

  // FrontendInterface

  int
  FrontendInterface::handle_request_noparams(
    FCGI::HttpRequest& request,
    FCGI::HttpResponse& response)
    throw(eh::Exception)
  {
    HTTP::ParamList params;

    try
    {
      // read parameters
      if(!request.args().empty())
      {
        FCGI::HttpRequest::parse_params(request.args(), params);
      }

      if(request.method() == FCGI::HttpRequest::RM_POST)
      {
        bool params_in_body = false;

        for(auto header_it = request.headers().begin();
          header_it != request.headers().end(); ++header_it)
        {
          if(header_it->name == String::AsciiStringManip::Caseless("content-type") &&
            FORMURL.start(header_it->value) &&
            NONTOKEN(header_it->value[FORMURL.str.length()]))
          {
            params_in_body = true;
            break;
          }
        }

        if(params_in_body)
        {
          FCGI::HttpRequest::parse_params(request.body(), params);
        }
      }

      request.set_params(std::move(params));
    }
    catch(const String::StringManip::InvalidFormatException&)
    {
      return 400; // bad request
    }

    return handle_request(request, response);
  }
}

