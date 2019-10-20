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

#ifndef UIDGENERATORADAPTERIMPL_HPP_
#define UIDGENERATORADAPTERIMPL_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/Singleton.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>

#include <Commons/ProcessControlVarsImpl.hpp>
#include <Commons/AtomicInt.hpp>

#include <xsd/Frontends/UIDGeneratorAdapterConfig.hpp>

#include "DescriptorHandlerPoller.hpp"
#include "UIDGeneratorAdapterLogger.hpp"

namespace AdServer
{
namespace Frontends
{
  class UIDGeneratorAdapterImpl:
    public Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    typedef xsd::AdServer::Configuration::UIDGeneratorAdapterConfigType
      Config;

    struct Stats: public ReferenceCounting::AtomicImpl
    {
      Stats()
        : processed_messages_count(0)
      {}

      Algs::AtomicInt processed_messages_count;
    };

    typedef ReferenceCounting::SmartPtr<Stats> Stats_var;

  public:
    UIDGeneratorAdapterImpl(
      Logging::Logger* logger,
      const Config& config)
      throw(eh::Exception);

  protected:
    typedef Generics::TaskGoal TaskBase;
    typedef ReferenceCounting::SmartPtr<TaskBase> Task_var;

    class FlushLogsTask: public TaskBase
    {
    public:
      FlushLogsTask(
        Generics::TaskRunner& task_runner,
        UIDGeneratorAdapterImpl* uid_generator_adapter_impl)
        throw();

      virtual void
      execute() throw();

    protected:
      UIDGeneratorAdapterImpl* uid_generator_adapter_impl_;
    };

    class PrintStatsTask: public TaskBase
    {
    public:
      PrintStatsTask(
        Generics::TaskRunner& task_runner,
        UIDGeneratorAdapterImpl* uid_generator_adapter_impl)
        throw();

      virtual void
      execute() throw();

    protected:
      UIDGeneratorAdapterImpl* uid_generator_adapter_impl_;
    };

  protected:
    void
    flush_logs_() throw();

    void
    print_stats_() throw();

  private:
    virtual
    ~UIDGeneratorAdapterImpl() throw()
    {}

  private:
    Logging::Logger_var logger_;
    Config config_;
    Generics::Planner_var scheduler_;
    Generics::TaskRunner_var task_runner_;

    DescriptorHandlerPoller_var poller_;
    UIDGeneratorAdapterLogger_var stat_logger_;
    Stats_var stats_;

    //Logging::LoggerCallbackHolder logger_callback_holder_;
    //StatHolder_var stats_;
  };

  typedef ReferenceCounting::QualPtr<UIDGeneratorAdapterImpl>
    UIDGeneratorAdapterImpl_var;
}
}

namespace AdServer
{
namespace Frontends
{
  inline
  UIDGeneratorAdapterImpl::FlushLogsTask::FlushLogsTask(
    Generics::TaskRunner& task_runner,
    UIDGeneratorAdapterImpl* uid_generator_adapter_impl)
      throw()
      : TaskBase(&task_runner),
        uid_generator_adapter_impl_(uid_generator_adapter_impl)
  {}

  inline void
  UIDGeneratorAdapterImpl::FlushLogsTask::execute()
    throw()
  {
    uid_generator_adapter_impl_->flush_logs_();
  }

  inline
  UIDGeneratorAdapterImpl::PrintStatsTask::PrintStatsTask(
    Generics::TaskRunner& task_runner,
    UIDGeneratorAdapterImpl* uid_generator_adapter_impl)
      throw()
      : TaskBase(&task_runner),
        uid_generator_adapter_impl_(uid_generator_adapter_impl)
  {}

  inline void
  UIDGeneratorAdapterImpl::PrintStatsTask::execute()
    throw()
  {
    uid_generator_adapter_impl_->print_stats_();
  }
}
}

#endif /*UIDGENERATORADAPTERIMPL_HPP_*/
