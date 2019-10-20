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

#ifndef USERINFOSVCS_USERBINDCONTROLLERIMPL_HPP_
#define USERINFOSVCS_USERBINDCONTROLLERIMPL_HPP_

#include <ReferenceCounting/ReferenceCounting.hpp>

#include <eh/Exception.hpp>

#include <Logger/Logger.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/Time.hpp>
#include <Sync/SyncPolicy.hpp>
#include <CORBACommons/CorbaAdapters.hpp>

#include <xsd/AdServerCommons/AdServerCommons.hpp>
#include <xsd/UserInfoSvcs/UserBindControllerConfig.hpp>

#include <Commons/CorbaObject.hpp>
#include <UserInfoSvcs/UserBindServer/UserBindServer.hpp>

#include <UserInfoSvcs/UserBindController/UserBindController_s.hpp>

namespace AdServer
{
namespace UserInfoSvcs
{
  // UserBindControllerImpl
  class UserBindControllerImpl:
    public CORBACommons::ReferenceCounting::CorbaRefCountImpl<
      POA_AdServer::UserInfoSvcs::UserBindController>,
    public Generics::CompositeActiveObject
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(NotReady, Exception);

    typedef xsd::AdServer::Configuration::UserBindControllerConfigType
      UserBindControllerConfig;

    UserBindControllerImpl(
      Generics::ActiveObjectCallback* callback,
      Logging::Logger* logger,
      const UserBindControllerConfig& user_bind_controller_config)
      throw(Exception);

    virtual AdServer::UserInfoSvcs::UserBindDescriptionSeq*
    get_session_description()
      throw(AdServer::UserInfoSvcs::UserBindController::ImplementationException,
        AdServer::UserInfoSvcs::UserBindController::NotReady);

    virtual CORBACommons::IProcessControl::ALIVE_STATUS
    get_status() throw();

    virtual char*
    get_comment() throw(CORBACommons::OutOfMemory);

  protected:
    typedef Generics::TaskGoal TaskBase;
    typedef ReferenceCounting::SmartPtr<TaskBase> Task_var;

    class InitUserBindSourceTask: public TaskBase
    {
    public:
      InitUserBindSourceTask(
        UserBindControllerImpl* user_bind_controller_impl,
        Generics::TaskRunner* task_runner)
        throw();

      virtual void
      execute() throw();

    protected:
      virtual
      ~InitUserBindSourceTask() throw ()
      {}

    protected:
      UserBindControllerImpl* user_bind_controller_impl_;
    };

    class CheckUserBindServerStateTask: public TaskBase
    {
    public:
      CheckUserBindServerStateTask(
        UserBindControllerImpl* user_info_manager_controller_impl,
        Generics::TaskRunner* task_runner)
        throw();

      virtual void
      execute() throw();

    protected:
      virtual
      ~CheckUserBindServerStateTask() throw ()
      {}

    protected:
      UserBindControllerImpl* user_bind_controller_impl_;
    };

  protected:
    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

    typedef std::set<unsigned long> ChunkIdSet;

    struct UserBindServerRef
    {
      AdServer::Commons::CorbaObject<
        AdServer::UserInfoSvcs::UserBindServer> user_bind_server;
      AdServer::Commons::CorbaObject<
        CORBACommons::IProcessControl> process_control;
      std::string host_name;
      ChunkIdSet chunks;
      bool ready;
    };

    typedef std::vector<UserBindServerRef> UserBindServerRefArray;

    class UserBindConfig: public ReferenceCounting::AtomicCopyImpl
    {
    public:
      UserBindConfig() throw()
        : all_ready(false),
          first_all_ready(false),
          common_chunks_number(0)
      {}

      bool all_ready;
      bool first_all_ready;
      unsigned long common_chunks_number;
      UserBindServerRefArray user_bind_servers;

    private:
      virtual
      ~UserBindConfig() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<UserBindConfig>
      UserBindConfig_var;

    typedef Sync::Policy::PosixThreadRW SyncPolicy;

  protected:
    virtual
    ~UserBindControllerImpl() throw ();

    bool
    get_user_bind_server_sources_(
      UserBindConfig* user_bind_config)
      throw(Exception);

    void
    admit_user_info_managers_() throw(Exception);

    // task functions implementation
    void
    init_user_bind_state_() throw();

    void
    check_user_bind_state_() throw();

    void
    fill_refs_() throw(Exception);

    void
    check_source_consistency_(
      UserBindConfig* user_bind_config)
      throw(Exception);

    /*
    UserInfoClusterControlImpl_var
    get_user_info_cluster_control_() throw();
    */

    void
    fill_user_bind_server_descr_seq_(
      AdServer::UserInfoSvcs::UserBindDescriptionSeq& user_bind_server_descr_seq)
      throw(AdServer::UserInfoSvcs::UserBindController::ImplementationException,
        AdServer::UserInfoSvcs::UserBindController::NotReady);

  protected:
    Generics::ActiveObjectCallback_var callback_;
    Logging::Logger_var logger_;

    mutable SyncPolicy::Mutex lock_;

    Generics::Planner_var scheduler_;
    Generics::TaskRunner_var task_runner_;

    CORBACommons::CorbaClientAdapter_var corba_client_adapter_;
    UserBindControllerConfig user_bind_controller_config_;

    UserBindConfig_var user_bind_config_;
  };

  typedef ReferenceCounting::SmartPtr<UserBindControllerImpl>
    UserBindControllerImpl_var;

  // UserBindClusterControlImpl
  class UserBindClusterControlImpl:
    public CORBACommons::ProcessControlDefault<
      POA_AdServer::UserInfoSvcs::UserBindClusterControl>
  {
  public:
    UserBindClusterControlImpl(
      UserBindControllerImpl* controller)
      throw();

    virtual CORBACommons::IProcessControl::ALIVE_STATUS
    is_alive() throw();

    virtual char*
    comment() throw(CORBACommons::OutOfMemory);

  protected:
    virtual
    ~UserBindClusterControlImpl() throw ();

    UserBindControllerImpl_var user_bind_controller_;
  };

  typedef ReferenceCounting::SmartPtr<UserBindClusterControlImpl>
    UserBindClusterControlImpl_var;
}
}

namespace AdServer
{
  namespace UserInfoSvcs
  {
    // UserBindControllerImpl::CheckUserBindServerStateTask
    inline
    UserBindControllerImpl::
    CheckUserBindServerStateTask::CheckUserBindServerStateTask(
      UserBindControllerImpl* user_info_manager_controller_impl,
      Generics::TaskRunner* task_runner)
      throw()
      : TaskBase(task_runner),
        user_bind_controller_impl_(
          user_info_manager_controller_impl)
    {}

    inline
    void
    UserBindControllerImpl::
    CheckUserBindServerStateTask::execute()
      throw()
    {
      user_bind_controller_impl_->check_user_bind_state_();
    }

    // UserBindControllerImpl::InitUserBindSourceTask
    inline
    UserBindControllerImpl::
    InitUserBindSourceTask::InitUserBindSourceTask(
      UserBindControllerImpl* user_info_manager_controller_impl,
      Generics::TaskRunner* task_runner)
      throw()
      : TaskBase(task_runner),
        user_bind_controller_impl_(
          user_info_manager_controller_impl)
    {}

    inline
    void
    UserBindControllerImpl::
    InitUserBindSourceTask::execute()
      throw()
    {
      user_bind_controller_impl_->init_user_bind_state_();
    }
  }
}

#endif /*USERINFOSVCS_USERBINDCONTROLLERIMPL_HPP_*/
