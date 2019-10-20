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

/**
 * @file RequestLogLoader.hpp
 */
#ifndef _REQUEST_LOG_LOADER_HPP_
#define _REQUEST_LOG_LOADER_HPP_

#include <Generics/TaskRunner.hpp>
#include <Generics/Values.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include <Logger/ActiveObjectCallback.hpp>
#include <LogCommons/FileReceiverFacade.hpp>

#include "RequestActionProcessor.hpp"
#include "PassbackContainer.hpp"
#include "TagRequestProcessor.hpp"
#include "RequestOperationLoader.hpp"

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class RequestInfoManagerStatsImpl;

    struct LogProcessingState: public ReferenceCounting::AtomicImpl
    {
      LogProcessingState()
        : interrupter(new LogProcessing::FileReceiverInterrupter())
      {}

      LogProcessing::FileReceiverInterrupter_var interrupter;

    protected:
      virtual
      ~LogProcessingState() throw()
      {}
    };

    typedef ReferenceCounting::AssertPtr<LogProcessingState>::Ptr
      LogProcessingState_var;

    struct InLog
    {
      std::string dir;
      unsigned int priority;

      InLog() throw ();
    };

    struct InLogs
    {
      InLog request;
      InLog impression;
      InLog click;
      InLog advertiser_action;
      InLog passback_impression;
      InLog tag_request;
      InLog request_operation;
    };

    class LogFetcherBase: public ReferenceCounting::AtomicImpl
    {
    public:
      LogFetcherBase(
        unsigned int priority,
        LogProcessing::FileReceiver* file_receiver)
        throw ();

      virtual
      Generics::Time
      check_files() throw() = 0;

      virtual void
      process(LogProcessing::FileReceiver::FileGuard* file_ptr) throw() = 0;

      unsigned int
      priority() const throw ();

      LogProcessing::FileReceiver_var
      file_receiver() throw ();

    protected:
      virtual
      ~LogFetcherBase() throw ()
      {}

    protected:
      const unsigned int priority_;
      LogProcessing::FileReceiver_var file_receiver_;
    };

    typedef ReferenceCounting::AssertPtr<LogFetcherBase>::Ptr LogFetcher_var;

    class RequestLogLoader:
      public virtual ReferenceCounting::AtomicImpl,
      public virtual Generics::CompositeActiveObject
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      RequestLogLoader(
        Generics::ActiveObjectCallback* callback,
        const InLogs& in_logs,
        UnmergedClickProcessor* unmerged_click_processor,
        RequestContainerProcessor* request_container_processor,
        AdvActionProcessor* adv_action_processor,
        PassbackVerificationProcessor* passback_verification_processor,
        TagRequestProcessor* tag_request_processor,
        RequestOperationProcessor* request_operation_processor,
        const Generics::Time& check_period,
        const Generics::Time& max_process_time,
        std::size_t threads_count,
        RequestInfoManagerStatsImpl* process_stats_values)
        throw (Exception);

    protected:
      virtual
      ~RequestLogLoader() throw()
      {}

      void
      process_file_() throw ();

    private:
      enum InLogType
      {
        RequestLogType = 0,
        ImpressionLogType,
        ClickLogType,
        AdvertiserActionLogType,
        PassbackImpressionLogType,
        TagRequestLogType,
        RequestOperationLogType,

        LogTypesCount
      };

      class OrderStrategy
      {
      public:
        typedef InLogType LogType;

        struct Key
        {
          std::size_t priority;
          Generics::Time time;

          friend bool
          operator< (
            const Key& arg1,
            const Key& arg2)
          {
            if (arg1.priority != arg2.priority)
            {
              return (arg1.priority > arg2.priority);
            }

            return (arg1.time < arg2.time);
          }
        };

      public:
        OrderStrategy(std::vector<std::size_t> priorities) throw ()
          : priorities_(priorities)
        {}

        Key
        key(
          LogType log_type,
          const std::string* log_file_name = nullptr) const
          throw ()
        {
          Key k;
          k.priority = priorities_[log_type];

          if (log_file_name)
          {
            LogProcessing::LogFileNameInfo name_info;
            LogProcessing::parse_log_file_name(*log_file_name, name_info);
            k.time = name_info.timestamp;
          }

          return k;
        }

      private:
        std::vector<std::size_t> priorities_;
      };

      typedef LogProcessing::FileReceiverFacade<OrderStrategy>
        FileReceiverFacade;

      typedef ReferenceCounting::SmartPtr<FileReceiverFacade>
        FileReceiverFacade_var;

      typedef std::map<InLogType, LogFetcher_var> LogFetchers;

    private:
      /// Callback for task_runner and logger for threads.
      Generics::ActiveObjectCallback_var log_errors_callback_;

      LogProcessingState_var processing_state_;

      Generics::Planner_var scheduler_;
      Generics::TaskRunner_var log_fetch_runner_;

      FileReceiverFacade_var file_receiver_facade_;
      LogFetchers log_fetchers_;
    };

    typedef ReferenceCounting::SmartPtr<RequestLogLoader> RequestLogLoader_var;
  }
}

#endif // _REQUEST_LOG_LOADER_HPP_
