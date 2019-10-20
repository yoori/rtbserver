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


#ifndef _FRONTENDCOMMONS_PROCCESSING_TEMPLATES_HPP_
#define _FRONTENDCOMMONS_PROCCESSING_TEMPLATES_HPP_

#include <string>
#include <map>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <String/TextTemplate.hpp>
#include <Generics/Hash.hpp>
#include <HTTP/Http.hpp>
#include <HTTP/HTTPCookie.hpp>

#include <Frontends/FrontendCommons/HTTPUtils.hpp>

#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

#include "FCGI.hpp"

namespace FrontendCommons
{
  typedef std::map<std::string, std::string> TokenValueMap;

  class PassbackTemplateKey
  {
  public:
    PassbackTemplateKey(
      const char* request_app_format,
      const char* passback_app_format)
      throw()
      : request_app_format_(request_app_format),
        passback_app_format_(passback_app_format),
        hash_(0)
    {
      Generics::Murmur64Hash hasher(hash_);
      hash_add(hasher, request_app_format_);
      hash_add(hasher, passback_app_format_);
    }

    bool operator==(const PassbackTemplateKey& right) const
    {
      return request_app_format_ == right.request_app_format_ &&
        passback_app_format_ == right.passback_app_format_;
    }

    size_t hash() const
    {
      return hash_;
    }

  protected:
    const std::string request_app_format_;
    const std::string passback_app_format_;
    size_t hash_;
  };

  struct PassbackProcessingTemplate: public ReferenceCounting::AtomicImpl
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    PassbackProcessingTemplate(
      const char* file,
      const char* content_type_val)
      throw(Exception);

    int
    instantiate(
      FCGI::HttpResponse& response,
      const char* url,
      AdServer::CampaignSvcs::UserStatus user_status,
      bool use_pub_pixels,
      unsigned long random,
      const TokenValueMap* args)
      throw(eh::Exception);

    String::TextTemplate::IStream text_template;
    const std::string content_type;

  protected:
    virtual ~PassbackProcessingTemplate() throw() {}
  };

  typedef ReferenceCounting::SmartPtr<PassbackProcessingTemplate>
    PassbackProcessingTemplate_var;

  typedef Generics::GnuHashTable<
    FrontendCommons::PassbackTemplateKey,
    FrontendCommons::PassbackProcessingTemplate_var>
    PassbackProcessingTemplateMap;
}


#endif //_FRONTENDCOMMONS_PROCCESSING_TEMPLATES_HPP_ 

