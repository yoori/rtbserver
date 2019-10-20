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

#ifndef XSLTFILECACHE_HPP
#define XSLTFILECACHE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

#include <String/TextTemplate.hpp>
#include <Generics/FileCache.hpp>
#include <Commons/Xslt/XslTransformer.hpp>

namespace AdServer
{
  class XsltTextTemplateArgs:
    public ReferenceCounting::AtomicImpl,
    public String::TextTemplate::Args
  {
  protected:
    virtual ~XsltTextTemplateArgs() throw() {}
  };

  typedef
    ReferenceCounting::QualPtr<XsltTextTemplateArgs>
    XsltTextTemplateArgs_var;

  typedef
    ReferenceCounting::ConstPtr<XsltTextTemplateArgs>
    CXsltTextTemplateArgs_var;

  class XsltFileUpdateStrategy
  {
  private:
    typedef std::unique_ptr<XslTransformer> XslTransformerPtr;

  public:
    struct Buffer
    {
      XslTransformerPtr transformer;
      Generics::Time timestamp;
    };

    XsltFileUpdateStrategy(
      const char* /*file*/) throw(eh::Exception)
    {
      assert(0);
    }

    XsltFileUpdateStrategy(
      const XsltTextTemplateArgs* args,
      const char* file) throw(eh::Exception);

    Buffer& get() throw();

    void update()
      throw(Generics::CacheExceptions::CacheException, eh::Exception);

  private:
    const std::string file_name_;
    CXsltTextTemplateArgs_var args_;
    Buffer xsl_transformer_;
  };

  typedef Generics::Cache<
    Generics::SimpleFileCheckStrategy,
    XsltFileUpdateStrategy>
    XsltFileCache;

  typedef XsltFileCache::Cache_var XsltFileCache_var;

  // XsltFileCacheFactory
  class XsltFileCacheFactory
  {
  public:
    XsltFileCacheFactory(const XsltTextTemplateArgs* args = 0);

    XsltFileCacheFactory(const XsltFileCacheFactory& init);

    virtual ~XsltFileCacheFactory() throw() {};

    XsltFileCache_var
    operator()(const char* file_name) throw(eh::Exception);

  private:
    CXsltTextTemplateArgs_var args_;
  };

  // XsltFileCacheManager
  class XsltFileCacheManager:
    public ReferenceCounting::AtomicImpl,
    public Generics::CacheManager<
      XsltFileCache,
      Generics::DefaultSizePolicy<std::string, XsltFileCache::Cache_var>,
      XsltFileCacheFactory>
  {
  public:
    XsltFileCacheManager(
      const XsltTextTemplateArgs* args,
      const Generics::Time& threshold_timeout,
      size_t bound_limit)
      throw(eh::Exception);

  protected:
    virtual ~XsltFileCacheManager() throw() {}
  };

  typedef ReferenceCounting::QualPtr<XsltFileCacheManager>
    XsltFileCacheManager_var;
}

#endif /*XSLTFILECACHE_HPP*/
