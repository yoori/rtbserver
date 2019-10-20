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

#ifndef CREATIVETEMPLATEARGS_HPP
#define CREATIVETEMPLATEARGS_HPP

#include <list>
#include <set>
#include <map>

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Time.hpp>
#include <String/TextTemplate.hpp>
#include <Sync/SyncPolicy.hpp>

#include <CampaignSvcs/CampaignCommons/CampaignTypes.hpp>

namespace TokenTemplateProperties
{
  const String::SubString START_TOKEN("##", 2);
  const String::SubString STOP_TOKEN("##", 2);
  const unsigned long MAX_TOKEN_DEPTH = 10;
}

namespace AdServer
{
  namespace CampaignSvcs
  {
    const String::AsciiStringManip::Caseless RELATIVE_URL_PREFIX("//");

    struct TokenMap: public std::map<long, std::string>
    {
      bool operator==(const TokenMap& right) const throw();
    };

    typedef std::map<std::string, OptionValue> OptionTokenValueMap;
    typedef std::map<std::string, std::string> TokenValueMap;

    struct CreativeInstantiateRule
    {
      CreativeInstantiateRule(bool use_empty_values = false) throw ()
        : use_empty_values_(use_empty_values)
      {}

      bool
      instantiate_relative_protocol_url(std::string& url) const throw();

      std::string url_prefix;
      std::string image_url;
      std::string publ_url;
      std::string click_url;
      std::string ad_server;
      std::string ad_image_server;
      std::string track_pixel_url;
      std::string action_pixel_url;
      std::string local_passback_prefix;
      std::string dynamic_creative_prefix;
      std::string passback_template_path_prefix;
      std::string passback_pixel_url;
      std::string user_bind_url;
      std::string pub_pixels_optin;
      std::string pub_pixels_optout;
      std::string script_instantiate_url;
      std::string iframe_instantiate_url;
      std::string direct_instantiate_url;
      std::string video_instantiate_url;

      OptionTokenValueMap tokens;
      bool use_empty_values_;
    };

    // runtime arguments, that depends on concrete creative
    struct CreativeInstantiateArgs
    {
      std::string full_dynamic_creative_prefix;
      std::string last_c_param;
    };

    typedef std::set<std::string> TokenSet;

    class BaseTokenProcessor: public virtual ReferenceCounting::AtomicImpl
    {
      typedef std::map<long, ReferenceCounting::SmartPtr<BaseTokenProcessor> >
        TokenProcessorMap;
      
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      DECLARE_EXCEPTION(InvalidValue, Exception);
      
      BaseTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);

      virtual bool
      instantiate(
        const OptionTokenValueMap& token_values,
        const TokenProcessorMap& token_processors,
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& result,
        int max_depth = TokenTemplateProperties::MAX_TOKEN_DEPTH)
        const throw(eh::Exception);

    public:
      static BaseTokenProcessor* default_token_processor(const char* token);
      
    protected:
      virtual ~BaseTokenProcessor() throw() {}

      virtual void post_process(
        const CreativeInstantiateRule& /*rule*/,
        const CreativeInstantiateArgs& /*creative_args*/,
        std::string& /*value*/) const
      {}
      
    private:
      std::string token_;
      TokenSet insert_restrictions_;
    };

    typedef ReferenceCounting::SmartPtr<BaseTokenProcessor> BaseTokenProcessor_var;

    class LinkTokenProcessor: public BaseTokenProcessor
    {
    public:
      LinkTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);
      
      virtual void post_process(
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& value) const;

    protected:
      virtual ~LinkTokenProcessor() throw() {}
    };

    class UrlTokenProcessor: public BaseTokenProcessor
    {
    public:
      UrlTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);
      
      virtual void post_process(
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& value) const;

    protected:
      virtual ~UrlTokenProcessor() throw() {}
    };

    class FileTokenProcessor: public BaseTokenProcessor
    {
    public:
      FileTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);
      
      virtual void post_process(
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& value) const;

    protected:
      virtual ~FileTokenProcessor() throw() {}
    };

    class PublisherUrlTokenProcessor: public BaseTokenProcessor
    {
    public:
      PublisherUrlTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);

      virtual void post_process(
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& value) const;

    protected:
      virtual ~PublisherUrlTokenProcessor() throw() {}
    };

    class PublisherFileTokenProcessor: public BaseTokenProcessor
    {
    public:
      PublisherFileTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);

      virtual void post_process(
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& value) const;

    protected:
      virtual ~PublisherFileTokenProcessor() throw() {}
    };

    class DynamicContentUrlTokenProcessor: public BaseTokenProcessor
    {
    public:
      DynamicContentUrlTokenProcessor(
        const char* token_name,
        const TokenSet& insert_restrictions)
        throw(eh::Exception);

      virtual void post_process(
        const CreativeInstantiateRule& rule,
        const CreativeInstantiateArgs& creative_args,
        std::string& value) const;

    protected:
      virtual ~DynamicContentUrlTokenProcessor() throw() {}
    };

    typedef std::map<long, BaseTokenProcessor_var> TokenProcessorMap;
  }
}

#endif /*CREATIVETEMPLATEARGS_HPP*/
