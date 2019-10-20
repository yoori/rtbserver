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
#include "XsltFileCache.hpp"

namespace AdServer
{
  //
  // XsltFileUpdateStrategy
  //
  XsltFileUpdateStrategy::XsltFileUpdateStrategy(
    const XsltTextTemplateArgs* args,
    const char* file)
    throw(eh::Exception)
    : file_name_(file), args_(ReferenceCounting::add_ref(args))
  {
    update();
  }

  XsltFileUpdateStrategy::Buffer&
  XsltFileUpdateStrategy::get() throw()
  {
    return xsl_transformer_;
  }

  void
  XsltFileUpdateStrategy::update()
    throw(Generics::CacheExceptions::CacheException, eh::Exception)
  {
    static const char* FUN = "XsltFileUpdateStrategy::update()";

    try
    {
      std::ifstream file(file_name_.c_str());
      if(!file)
      {
        Stream::Error ostr;
        ostr << FUN << ": Can't open xslt template '" << file_name_ << "'.";
        throw Generics::CacheExceptions::CacheException(ostr);
      }

      if(args_.in())
      {
        String::TextTemplate::IStream xslt_text_template(
          file, String::SubString("##"), String::SubString("##"));
        std::string instantiated_template = xslt_text_template.instantiate(*args_);
        Stream::Parser xsl_content(instantiated_template);
        xsl_transformer_.transformer.reset(
          new XslTransformer(xsl_content, file_name_.c_str()));
      }
      else
      {
        xsl_transformer_.transformer.reset(
          new XslTransformer(file_name_.c_str()));
      }

      xsl_transformer_.timestamp = Generics::Time::get_time_of_day();
    }
    catch(const String::TextTemplate::TextTemplException& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": Can't init Xslt template by '" << file_name_ <<
        "': " << ex.what();
      throw Generics::CacheExceptions::CacheException(ostr);
    }
    catch(const AdServer::XslTransformer::Exception& ex)
    {
      Stream::Error ostr;
      ostr << FUN << ": Can't init Xslt template by '" << file_name_ <<
        "': " << ex.what();
      throw Generics::CacheExceptions::CacheException(ostr);
    }
  }

  //
  // XsltFileCacheFactory
  //
  XsltFileCacheFactory::XsltFileCacheFactory(
    const XsltTextTemplateArgs* args)
    : args_(ReferenceCounting::add_ref(args))
  {}

  XsltFileCacheFactory::XsltFileCacheFactory(
    const XsltFileCacheFactory& init)
    : args_(init.args_)
  {}

  XsltFileCache_var
  XsltFileCacheFactory::operator()(const char* file_name)
    throw(eh::Exception)
  {
    return XsltFileCache_var(new XsltFileCache(
      new Generics::SimpleFileCheckStrategy(file_name),
      new XsltFileUpdateStrategy(args_, file_name)));
  }

  //
  // XsltFileCacheManager
  //
  XsltFileCacheManager::XsltFileCacheManager(
    const XsltTextTemplateArgs* args,
    const Generics::Time& threshold_timeout,
    size_t bound_limit)
    throw(eh::Exception)
    : Generics::CacheManager<
        XsltFileCache,
        Generics::DefaultSizePolicy<std::string, XsltFileCache::Cache_var>,
        XsltFileCacheFactory>(
          XsltFileCacheFactory(args),
          threshold_timeout,
          bound_limit)
  {}
}
