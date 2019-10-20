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

/// @file AdFrontend/FrontendStat.cpp
#include <Controlling/StatsDumper/StatsDumper.hpp>

#include "FrontendStatistic.hpp"

namespace Aspect
{
  const char FRONTEND_COMMONS[] = "FrontendCommons";
}

namespace
{
  class DumpStatsTask : public Generics::GoalTask
  {
  public:
    DumpStatsTask(
      AdServer::Controlling::StatsDumper& stats_dumper,
      Logging::Logger* logger,
      Generics::Planner* planner,
      Generics::TaskRunner* task_runner,
      const Generics::Time& update_period,
      AdServer::FrontendStat& stats_values)
      throw ()
      : Generics::GoalTask(planner, task_runner),
        logger_(ReferenceCounting::add_ref(logger)),
        update_period_(update_period),
        stats_dumper_(stats_dumper),
        stats_values_(stats_values)
    {}

    virtual void
    execute() throw ()
    {
      static const char FUN[] = "DumpStatsTask::execute";
      try
      {
        Generics::Values_var snapshot = stats_values_.extract_stats_values();
        stats_dumper_.dump_statistics(*snapshot);
      }
      catch (const AdServer::Controlling::StatsDumper::Exception& ex)
      {
        logger_->sstream(
          Logging::Logger::EMERGENCY,
          Aspect::FRONTEND_COMMONS,
          ex.code()) << FUN << "(): caught Controlling::StatsDumper::Exception: " << ex.what();
      }
      catch (eh::Exception& ex)
      {
        logger_->sstream(Logging::Logger::EMERGENCY,
          Aspect::FRONTEND_COMMONS, "ADS-IMPL-143") << FUN <<
          "(): dump statistics failed: " << ex.what();
      }
      schedule();
    }

    virtual void
    schedule() throw ()
    {
      try
      {
        Generics::GoalTask::schedule(
          Generics::Time::get_time_of_day() + update_period_);
      }
      catch (const eh::Exception& ex)
      {
        logger_->sstream(Logging::Logger::EMERGENCY,
          Aspect::FRONTEND_COMMONS, "ADS-IMPL-143") << "DumpStatsTask::schedule(): failed: " <<
          ex.what();
      }
    }

  protected:
    virtual ~DumpStatsTask() throw ()
    {}

  private:
    Logging::Logger_var logger_;
    const Generics::Time update_period_;
    AdServer::Controlling::StatsDumper& stats_dumper_;
    AdServer::FrontendStat& stats_values_;
  };

  typedef ReferenceCounting::SmartPtr<DumpStatsTask> DumpStatsTask_var;

}

namespace AdServer
{
  FrontendStat::FrontendStat(
    Logging::Logger* logger,
    const CORBACommons::CorbaObjectRef& stats_collector_ref,
    Generics::Planner* scheduler_ptr,
    const Generics::Time& dump_period,
    Generics::ActiveObjectCallback* callback,
    const char* host_name)
    throw(Exception)
    : logger_(ReferenceCounting::add_ref(logger)),
      stats_dumper_(stats_collector_ref, host_name)
  {
    static const char* FUN = "FrontendStat::FrontendStat()";

    try
    {
      Generics::Planner_var planner;
      if (scheduler_ptr)
      {
        planner = ReferenceCounting::add_ref(scheduler_ptr);
      }
      else
      {
        planner = new Generics::Planner(callback);
      }
      add_child_object(planner.in());

      Generics::TaskRunner_var task_runner(
        new Generics::TaskRunner(callback, 1));
      add_child_object(task_runner.in());

      DumpStatsTask_var msg = new DumpStatsTask(stats_dumper_,
        logger, planner, task_runner, dump_period, *this);
      msg->schedule();
    }
    catch(const eh::Exception& e)
    {
      Stream::Error ostr;
      ostr << FUN <<
        ": Controlling::StatsCollector::can't schedule dump task: "
        << e.what();
      throw Exception(ostr);
    }
  }

}
