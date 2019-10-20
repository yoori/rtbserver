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

#ifndef EXPRESSIONMATCHER_CONVERSIONPROCESSOR_HPP_
#define EXPRESSIONMATCHER_CONVERSIONPROCESSOR_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>

#include <Commons/UserInfoManip.hpp>
#include <LogCommons/LogHolder.hpp>

#include "InventoryActionProcessor.hpp"

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class ConversionProcessor:
      public InventoryActionProcessor,
      public virtual AdServer::LogProcessing::CompositeLogHolder,
      public virtual Generics::CompositeActiveObject,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      struct Config: public ReferenceCounting::AtomicImpl
      {
        typedef std::vector<unsigned long> CcgIdArray;

        struct Action
        {
          unsigned long action_id;
          CcgIdArray ccgs;
        };

        typedef std::map<unsigned long, Action> ChannelActionMap;

        ChannelActionMap channel_actions;

      protected:
        virtual ~Config() throw() = default;
      };

      typedef ReferenceCounting::SmartPtr<Config> Config_var;

    public:
      ConversionProcessor(
        Logging::Logger* logger,
        Generics::ActiveObjectCallback* callback,
        const AdServer::LogProcessing::LogFlushTraits& action_request_flush_traits)
        throw(eh::Exception);

      virtual
      void process_request(const InventoryInfo&)
        throw(InventoryActionProcessor::Exception);

      virtual
      void process_user(const InventoryUserInfo&)
        throw(InventoryActionProcessor::Exception);

    protected:
      typedef std::vector<unsigned long> ActionIdArray;

      class ConvLogger;

      typedef ReferenceCounting::SmartPtr<ConvLogger>
        ConvLogger_var;

      class FlushLogsTaskMessage: public Generics::TaskGoal
      {
      public:
        FlushLogsTaskMessage(
          ConversionProcessor* action_processor,
          Generics::TaskRunner* task_runner)
          throw(eh::Exception);

        virtual void
        execute() throw();

      protected:
        virtual
        ~FlushLogsTaskMessage() throw() = default;

      protected:
        ConversionProcessor* action_processor_;
      };

    protected:
      typedef Sync::Policy::PosixThreadRW SyncPolicy;

    protected:
      void
      flush_logs_() throw();

    protected:
      Logging::Logger_var logger_;
      Generics::ActiveObjectCallback_var callback_;
      InventoryActionProcessor_var inventory_processor_;

      Generics::TaskRunner_var task_runner_;
      Generics::Planner_var scheduler_;
      ConvLogger_var conv_logger_;

      SyncPolicy::Mutex config_lock_;
      Config_var config_;
    };

    typedef ReferenceCounting::SmartPtr<ConversionProcessor>
      ConversionProcessor_var;
  }
}

#endif /*EXPRESSIONMATCHER_CONVERSIONPROCESSOR_HPP_*/
