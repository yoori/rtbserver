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


#pragma once

#include <list>
#include <pthread.h>
#include <Sync/PosixLock.hpp>
#include <Sync/Condition.hpp>
#include <Generics/Time.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include <Commons/DelegateActiveObject.hpp>
#include <Frontends/FrontendCommons/FrontendInterface.hpp>

namespace AdServer
{
namespace Frontends
{

  class WorkerStatsObject: public Commons::DelegateActiveObject
  { 
    static const Generics::Time STATS_TIMEOUT;
    
  public:

    WorkerStatsObject(
      Logging::Logger* logger,
      Generics::ActiveObjectCallback* callback)
      throw (eh::Exception);

    void
    work_() throw();

    virtual void
    terminate_() throw();

    void incr_workers();

    void dec_workers();

  protected:
    virtual
    ~WorkerStatsObject() throw();

  private:
    typedef Sync::Policy::PosixThread WorkerStatsSyncPolicy;

    WorkerStatsSyncPolicy::Mutex worker_stats_lock_;
    WorkerStatsSyncPolicy::Mutex worker_cond_lock_;
    Sync::Conditional workers_condition_;
    
    Logging::Logger_var logger_;

    unsigned long workers_count_;
    unsigned long workers_inerval_max_;
    unsigned long workers_max_;
  };

  typedef ReferenceCounting::SmartPtr<WorkerStatsObject>
      WorkerStatsObject_var;
      
  class Acceptor:
    public Generics::CompositeActiveObject,
    public ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    Acceptor(
      Logging::Logger* logger,
      FrontendCommons::FrontendInterface* frontend,
      Generics::ActiveObjectCallback* callback,
      const char* bind_address,
      unsigned long backlog,
      unsigned long join_threads)
      throw (eh::Exception);

    FrontendCommons::FrontendInterface*
    handler() throw();

    Logging::Logger*
    logger() throw();

    virtual void
    activate_object()
      throw (Exception, eh::Exception);

    virtual void
    deactivate_object()
      throw (Exception, eh::Exception);

    virtual void
    wait_object()
      throw (Exception, eh::Exception);

  protected:
    struct State;
    typedef ReferenceCounting::SmartPtr<State> State_var;

    class Worker;
    class AcceptActiveObject;
    class JoinActiveObject;

  protected:
    virtual
    ~Acceptor() throw ();

  private:
    Generics::ActiveObjectCallback_var callback_;
    Logging::Logger_var logger_;
    FrontendCommons::Frontend_var frontend_;
    Generics::ActiveObject_var join_active_object_;
    WorkerStatsObject_var worker_stats_object_;
    State_var state_;
  };
}
}
