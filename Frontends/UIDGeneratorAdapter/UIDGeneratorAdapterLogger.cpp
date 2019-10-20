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

#include <sstream>

#include <LogCommons/GenericLogIoImpl.hpp>
#include "UIDGeneratorAdapterLogger.hpp"

namespace AdServer
{
namespace Frontends
{
  class UIDGeneratorAdapterLogger::RequestLogger:
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    virtual void
    process_request(
      const UIDGeneratorAdapterLogger::RequestInfo& request_info)
      throw(Exception) = 0;

  protected:
    virtual
    ~RequestLogger() throw()
    {}
  };

  namespace
  {
    const char KEYWORD_HIT_STAT_LOGGER[] = "KeywordHitStatLogger";

    // UIDGeneratorAdapterLogger::KeywordHitStatLogger
    class KeywordHitStatLogger:
      public virtual UIDGeneratorAdapterLogger::RequestLogger,
      public virtual AdServer::LogProcessing::LogHolderPool<
        AdServer::LogProcessing::KeywordHitStatTraits>
    {
    public:
      DECLARE_EXCEPTION(Exception, UIDGeneratorAdapterLogger::Exception);

      KeywordHitStatLogger(
        const AdServer::LogProcessing::LogFlushTraits& flush_traits)
        throw(Exception)
        : AdServer::LogProcessing::LogHolderPool<
            AdServer::LogProcessing::KeywordHitStatTraits>(flush_traits)
      {};

      void
      process_request(
        const UIDGeneratorAdapterLogger::RequestInfo& request_info)
        throw(Exception)
      {
        CollectorT::KeyT key(request_info.time);
        CollectorT::DataT data;

        for(auto keyword_it = request_info.keywords.begin();
          keyword_it != request_info.keywords.end(); ++keyword_it)
        {
          CollectorT::DataT::KeyT inner_key(*keyword_it);
          data.add(inner_key, CollectorT::DataT::DataT(1));
        }

        add_record(key, data);
      }

    protected:
      virtual
      ~KeywordHitStatLogger() throw()
      {};
    };
  }

  UIDGeneratorAdapterLogger::UIDGeneratorAdapterLogger(
    Logging::Logger* logger,
    const AdServer::LogProcessing::LogFlushTraits& keyword_hit_stat_flush_traits)
    throw(Exception)
    : logger_(ReferenceCounting::add_ref(logger))
  {
    if(keyword_hit_stat_flush_traits.period != Generics::Time::ZERO)
    {
      ReferenceCounting::SmartPtr<KeywordHitStatLogger> keyword_hit_stat_logger =
        new KeywordHitStatLogger(keyword_hit_stat_flush_traits);

      add_child_log_holder(keyword_hit_stat_logger);

      request_loggers_.push_back(keyword_hit_stat_logger);
    }
  }

  UIDGeneratorAdapterLogger::~UIDGeneratorAdapterLogger() throw()
  {}

  void
  UIDGeneratorAdapterLogger::process_request(
    const RequestInfo& request_info)
    throw(Exception)
  {
    for(auto request_logger_it = request_loggers_.begin();
      request_logger_it != request_loggers_.end(); ++request_logger_it)
    {
      (*request_logger_it)->process_request(request_info);
    }
  }
}
}
