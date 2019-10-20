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

#ifndef ADSERVER_FRONTENDS_UIDGENERATORADAPTER_UIDGENERATORADAPTERLOGGER_HPP_
#define ADSERVER_FRONTENDS_UIDGENERATORADAPTER_UIDGENERATORADAPTERLOGGER_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>

#include <LogCommons/LogHolder.hpp>
#include <LogCommons/KeywordHitStat.hpp>

namespace AdServer
{
namespace Frontends
{
  class UIDGeneratorAdapterLogger:
    public virtual AdServer::LogProcessing::CompositeLogHolder
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    typedef std::vector<AdServer::Commons::StringHolder_var> KeywordArray;

    struct RequestInfo
    {
      Generics::Time time;
      KeywordArray keywords;
    };

    class RequestLogger;

  public:
    UIDGeneratorAdapterLogger(
      Logging::Logger* logger,
      const AdServer::LogProcessing::LogFlushTraits& keyword_hit_stat_log_params)
        throw(Exception);

    void
    process_request(
      const RequestInfo& request_info)
      throw(Exception);

  protected:
    typedef ReferenceCounting::SmartPtr<RequestLogger>
      RequestLogger_var;

    typedef std::list<RequestLogger_var> RequestLoggerList;

  protected:
    virtual
    ~UIDGeneratorAdapterLogger() throw();

  protected:
    Logging::Logger_var logger_;
    RequestLoggerList request_loggers_;
  };

  typedef ReferenceCounting::SmartPtr<UIDGeneratorAdapterLogger>
    UIDGeneratorAdapterLogger_var;
}
}

#endif /*ADSERVER_FRONTENDS_UIDGENERATORADAPTER_UIDGENERATORADAPTERLOGGER_HPP_*/
