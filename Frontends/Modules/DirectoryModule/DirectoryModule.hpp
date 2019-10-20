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

#ifndef ADSERVER_FRONTENDS_DIRECTORYMODULE_HPP
#define ADSERVER_FRONTENDS_DIRECTORYMODULE_HPP

#include <string>

#include <eh/Exception.hpp>

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/ActiveObject.hpp>
#include <Logger/Logger.hpp>
#include <Logger/DistributorLogger.hpp>
#include <Logger/ActiveObjectCallback.hpp>
#include <Generics/MMap.hpp>
#include <HTTP/Http.hpp>

#include <FrontendCommons/BoundedCache.hpp>
#include <Frontends/FrontendCommons/HTTPExceptions.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

namespace AdServer
{
  
  /*
   *   VersioningDirectory
   **
   *   path/file -> path/<file without version suffix>.<greatest version>
   */
  class DirectoryModule:
    private Logging::LoggerCallbackHolder,
    private FrontendCommons::HTTPExceptions,
    public virtual FrontendCommons::FrontendInterface,
    public virtual ReferenceCounting::AtomicImpl
  {
    typedef FrontendCommons::HTTPExceptions::Exception Exception;
  public:
    DirectoryModule(
      Configuration* frontend_config_,
      Logging::Logger* logger)
      throw(eh::Exception);

    virtual bool
    will_handle(const String::SubString& uri) throw ();

    virtual int
    handle_request(
      const FCGI::HttpRequest& request,
      FCGI::HttpResponse& response) throw ();

    /** Performs initialization for the module child process. */
    virtual void
    init() throw (eh::Exception);

    /** Performs shutdown for the module child process. */
    virtual void
    shutdown() throw ();

  protected:
    virtual
    ~DirectoryModule() throw () = default;

  private:
    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

    /* versioned files cache
     * requirements: file content can't be changed after creation
     */
    class FileContent: public ReferenceCounting::AtomicImpl
    {
    public:
      FileContent(
        const char* file_name,
        const void* buf,
        unsigned long buf_len)
        throw(eh::Exception)
        : file_name_(file_name),
          buf_(buf_len),
          length_(buf_len)
      {
        ::memcpy(buf_.get(), buf, buf_len);
      }

      const std::string& file_name() const throw()
      {
        return file_name_;
      }

      const void* data() const throw()
      {
        return buf_.get();
      }

      unsigned long length() const throw()
      {
        return length_;
      }

    protected:
      virtual ~FileContent() throw() {}

    private:
      const std::string file_name_;
      const Generics::ArrayAutoPtr<char> buf_;
      const unsigned long length_;
    };

    typedef ReferenceCounting::SmartPtr<FileContent>
      FileContent_var;

    class VersionedFileCacheConfiguration
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      struct Holder
      {
        Generics::Time timestamp;
        FileContent_var content;
      };

    public:
      VersionedFileCacheConfiguration(const Generics::Time& check_period =
        Generics::Time(10)) throw();

      bool update_required(
        const Generics::StringHashAdapter& key,
        const Holder& file_state) throw();

      Holder update(
        const Generics::StringHashAdapter& key,
        const Holder* old_holder)
        throw(Exception);

      unsigned long size(const Holder& file_state) const throw();

      FileContent_var adapt(const Holder& file_state) const throw();

    private:
      Generics::Time check_period_;
    };

    typedef BoundedCache<
      Generics::StringHashAdapter,
      FileContent_var,
      VersionedFileCacheConfiguration,
      AdServer::Commons::RCHash2Args>
      VersionedFileCache;

    typedef ReferenceCounting::SmartPtr<VersionedFileCache>
      VersionedFileCache_var;

    struct Directory: public ReferenceCounting::AtomicImpl
    {
      std::string root;
      VersionedFileCache_var cache;

    protected:
      virtual ~Directory() throw() {}
    };

    typedef ReferenceCounting::SmartPtr<Directory> Directory_var;

    typedef std::map<std::string, Directory_var> DirAliasMap;

    typedef Configuration::FeConfig::ContentFeConfiguration_type
      ContentFeConfiguration;

    typedef std::unique_ptr<ContentFeConfiguration> ConfigPtr;

  private:
    void parse_configs_() throw(Exception);

  private:
    DirAliasMap directories_;
    ConfigPtr config_;
    Configuration_var frontend_config_;
  };
}

#endif /*ADSERVER_FRONTENDS_DIRECTORYMODULE_HPP*/
