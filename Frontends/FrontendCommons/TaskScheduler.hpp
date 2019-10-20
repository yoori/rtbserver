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

#ifndef _FRONTENDCOMMONS_TASKSCHDULER_HPP_
#define _FRONTENDCOMMONS_TASKSCHDULER_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>

namespace FrontendCommons
{
  class TaskScheduler:
    public virtual ReferenceCounting::AtomicImpl,
    public virtual Generics::CompositeActiveObject
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  public:
    TaskScheduler(
      Generics::ActiveObjectCallback* active_object_callback,
      Generics::TaskRunner* task_runner = 0)
      throw(Exception);

    Generics::TaskRunner*
    task_runner() throw ();

    Generics::Planner*
    planner() throw ();

    void
    schedule(Generics::Goal* task, const Generics::Time& time)
      throw (Exception);

  private:
    typedef ReferenceCounting::SmartPtr<Generics::Task> Task_var;

    Generics::ActiveObjectCallback_var active_object_callback_;
    Generics::Planner_var scheduler_;
    Generics::TaskRunner_var task_runner_;
  };

  typedef ReferenceCounting::SmartPtr<TaskScheduler> TaskScheduler_var;

}

namespace FrontendCommons
{
  inline
  TaskScheduler::TaskScheduler(
    Generics::ActiveObjectCallback* active_object_callback,
    Generics::TaskRunner* task_runner)
    throw(Exception)
    : active_object_callback_(ReferenceCounting::add_ref(active_object_callback)),
      scheduler_(new Generics::Planner(active_object_callback_)),
      task_runner_(ReferenceCounting::add_ref(task_runner))
  {
    if(task_runner_.in() == 0)
    {
      try
      {
        task_runner_ = new Generics::TaskRunner(active_object_callback_, 1);
      }
      catch(const eh::Exception& ex)
      {
        Stream::Error ostr;
        ostr << "TaskScheduler::TaskScheduler(): "
          << "Can't create TaskRunner. eh::Exception caught. "
          << ": " << ex.what() << ".";
        throw Exception(ostr);
      }
    }

    try
    {
      add_child_object(task_runner_);
      add_child_object(scheduler_);
    }
    catch(const Generics::CompositeActiveObject::Exception& ex)
    {
      Stream::Error ostr;
      ostr << "TaskScheduler::TaskScheduler(): "
        << "CompositeActiveObject::Exception caught. "
        << ": " << ex.what() << ".";
      throw Exception(ostr);
    }
  }

  inline
  Generics::TaskRunner*
  TaskScheduler::task_runner() throw ()
  {
    return task_runner_;
  }

  inline
  Generics::Planner*
  TaskScheduler::planner() throw ()
  {
    return scheduler_;
  }

  inline
  void
  TaskScheduler::schedule(
    Generics::Goal* task, const Generics::Time& time)
    throw (Exception)
  {
    try
    {
      scheduler_->schedule(task, time);
    }
    catch(const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << "TaskScheduler::schedule(): "
        << "eh::Exception caught. "
        << ": " << ex.what() << ".";
      throw Exception(ostr);
    }
  }
}

#endif /*_FRONTENDCOMMONS_TASKSCHDULER_HPP_*/
