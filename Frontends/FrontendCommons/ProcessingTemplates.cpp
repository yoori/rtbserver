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

#include <fstream>
#include "ProcessingTemplates.hpp"

namespace FrontendCommons
{
  PassbackProcessingTemplate::PassbackProcessingTemplate(
    const char* file,
    const char* content_type_val)
    throw(Exception)
    : content_type(content_type_val)
  {
    static const char* FUN = "PassbackProcessingTemplate::PassbackProcessingTemplate()";

    std::fstream fstr(file, std::ios::in);
    if(!fstr.is_open())
    {
      Stream::Error ostr;
      ostr << FUN << ": can't init open file '" << file << "'";
      throw Exception(ostr);
    }

    try
    {
      text_template.init(fstr, String::SubString("##"), String::SubString("##"));
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": can't init template by file '" << file << "': " <<
        ex.what();
      throw Exception(ostr);
    }
  }

  int
  PassbackProcessingTemplate::instantiate(
    FCGI::HttpResponse& response,
    const char* passback_url,
    AdServer::CampaignSvcs::UserStatus user_status,
    bool use_pub_pixels,
    unsigned long random,
    const TokenValueMap* args)
    throw(eh::Exception)
  {
    char random_str[std::numeric_limits<unsigned long>::digits10 + 3];
    response.set_content_type(content_type);
    TokenValueMap args_copy;
    if(args)
    {
      args_copy = *args;
    }
    if(!String::StringManip::int_to_str(random, random_str, sizeof(random_str)))
    {
      throw Exception("can not process random value");
    }
    args_copy.insert(std::make_pair("URL", passback_url));
    args_copy.insert(std::make_pair(
      "USERSTATUS",
      user_status == AdServer::CampaignSvcs::US_OPTIN ? "1" : (
        user_status == AdServer::CampaignSvcs::US_OPTOUT ? "-1" : "0")));
    args_copy.insert(std::make_pair(
      "PUBPIXELS", use_pub_pixels ? "1" : "0"));
    args_copy.insert(std::make_pair("RANDOM", random_str));

    String::TextTemplate::ArgsContainer<
      TokenValueMap,
      String::TextTemplate::ArgsContainerStringAdapter> args_cont(&args_copy);
    String::TextTemplate::DefaultValue args_with_default(&args_cont);
    String::TextTemplate::ArgsEncoder args_encoder(&args_with_default);
    std::string res = text_template.instantiate(args_encoder);
    response.get_output_stream().write(res.data(), res.size());

    return 200;
  }

}

