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

#include <string>
#include <set>
#include <Generics/ActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <ChannelSvcs/ChannelServer/ChannelServer.hpp>
#include <ChannelSvcs/ChannelServer/ChannelUpdateBase.hpp>
#include <ChannelSvcs/ChannelServer/ChannelServer_s.hpp>

#ifndef DUMMY_CHANNEL_SERVER_HPP_
#define DUMMY_CHANNEL_SERVER_HPP_

namespace AdServer
{
  namespace ChannelSvcs
  {
    typedef AdServer::ChannelSvcs::ChannelUpdate_v33 ChannelCurrent;
  }

  namespace UnitTests
  {
    class DummyChannelServer:
      public virtual CORBACommons::ReferenceCounting::ServantImpl
      <POA_AdServer::ChannelSvcs::ChannelServer>,
      protected virtual Generics::ActiveObjectCallback
    {
    private:
      virtual ~DummyChannelServer() throw() {};

    public:
      DummyChannelServer(
        CORBACommons::OrbShutdowner* shutdowner,
        unsigned long time,
        unsigned long count_matching,
        bool verbose) throw();

      //
      // IDL:AdServer/ChannelSvcs/ChannelServer/match:1.0
      //
      virtual void match(
        const ::AdServer::ChannelSvcs::ChannelServerBase::MatchQuery& query,
        ::AdServer::ChannelSvcs::ChannelServer::MatchResult_out result)
        throw(AdServer::ChannelSvcs::ImplementationException);

      //
      // IDL:AdServer/ChannelSvcs/ChannelServer/match:1.0
      //
      virtual void
        get_ccg_traits(
          const ::AdServer::ChannelSvcs::ChannelIdSeq& query,
          ::AdServer::ChannelSvcs::ChannelServer::TraitsResult_out result)
        throw(AdServer::ChannelSvcs::ImplementationException,
              AdServer::ChannelSvcs::NotConfigured);

      //
      // IDL:AdServer/ChannelSvcs/ChannelServerControl/set_sources:1.0
      //
      virtual void set_sources(
        const ::AdServer::ChannelSvcs::
          ChannelServerControl::DBSourceInfo& db_info,
        const ::AdServer::ChannelSvcs::ChunkKeySeq& sources)
        throw();

      //
      // IDL:AdServer/ChannelSvcs/ChannelServerControl/set_proxy_sources:1.0
      //
      virtual void set_proxy_sources(
        const ::AdServer::ChannelSvcs::ChannelServerControl::ProxySourceInfo&
          poxy_info,
        const ::AdServer::ChannelSvcs::ChunkKeySeq& sources)
        throw();

      //
      // IDL:AdServer/ChannelSvcs/ChannelServerControl/get_queries_counter:1.0
      //
      ::CORBA::ULong get_queries_counter() throw();

      //
      // IDL:AdServer/ChannelSvcs/ChannelProxy/get_count_chunks:1.0
      //
      ::CORBA::ULong get_count_chunks()
        throw(AdServer::ChannelSvcs::ImplementationException);

      //
      // IDL:AdServer/ChannelSvcs/ChannelServerControl/update_triggers:1.0
      //
      void update_triggers(
        const ::AdServer::ChannelSvcs::ChannelIdSeq& ids,
        ::AdServer::ChannelSvcs::ChannelCurrent::UpdateData_out result)
        throw(AdServer::ChannelSvcs::ImplementationException);

      virtual void activate_object()
        throw(Generics::ActiveObject::AlreadyActive,
              Generics::ActiveObject::Exception, eh::Exception);

      virtual void deactivate_object()
        throw(Generics::ActiveObject::Exception, eh::Exception);

      virtual void wait_object()
        throw(Generics::ActiveObject::Exception, eh::Exception);

      virtual void report_error(
        Generics::ActiveObjectCallback::Severity severity,
        const String::SubString& description,
        const char* error_code = 0) throw();

      virtual bool active() throw(eh::Exception);

      void finish() throw();

    private:
      class FinishTask: public Generics::TaskGoal
      {
      public:
        FinishTask(
          DummyChannelServer* server_impl,
          Generics::TaskRunner* task_runner) throw();

        virtual ~FinishTask() throw();
        virtual void execute() throw();

      private:
        DummyChannelServer* server_impl_;
      };
      typedef ReferenceCounting::SmartPtr<FinishTask> Task_var;

      typedef std::set<unsigned long> TriggerSet;

    private:
      bool active_;
      bool verbose_;
      unsigned long count_matching_;
      CORBACommons::OrbShutdowner_var shutdowner_;
      Generics::Planner_var scheduler_;
      Generics::TaskRunner_var task_runner_;
      TriggerSet response_info_;
    };
    typedef ReferenceCounting::SmartPtr<DummyChannelServer>
      DummyChannelServer_var;
  }
}

namespace AdServer
{
  namespace UnitTests
  {
    inline
    bool DummyChannelServer::active() throw(eh::Exception)
    {
      return active_;
    }

    inline
    DummyChannelServer::FinishTask::FinishTask(
      DummyChannelServer* server_impl,
      Generics::TaskRunner* task_runner) 
      throw()
      : Generics::TaskGoal(task_runner), server_impl_(server_impl)
    {
    }

    inline
    DummyChannelServer::FinishTask::~FinishTask() throw()
    {
    }

    inline
    void DummyChannelServer::FinishTask::execute() throw()
    {
      server_impl_->finish();
    }
    
  }
}
#endif //DUMMY_CHANNEL_SERVER_HPP_
