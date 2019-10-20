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

#ifndef USERINFOSVCS_USERBINDSERVERIMPL_HPP
#define USERINFOSVCS_USERBINDSERVERIMPL_HPP

#include <list>
#include <vector>
#include <string>

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/PtrHolder.hpp>

#include <Logger/Logger.hpp>
#include <Generics/ActiveObject.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/Time.hpp>

#include <CORBACommons/ServantImpl.hpp>

#include <Commons/CorbaConfig.hpp>
#include <Commons/AccessActiveObject.hpp>
#include <Commons/UserIdBlackList.hpp>

#include <xsd/AdServerCommons/AdServerCommons.hpp>
#include <xsd/UserInfoSvcs/UserBindServerConfig.hpp>

#include <UserInfoSvcs/UserBindServer/UserBindServer_s.hpp>

#include "UserBindContainer.hpp"
#include "BindRequestContainer.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  /**
   * Implementation of UserBindServer.
   */
  class UserBindServerImpl:
    public virtual CORBACommons::ReferenceCounting::ServantImpl<
      POA_AdServer::UserInfoSvcs::UserBindServer>,
    public virtual Generics::CompositeActiveObject
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    typedef xsd::AdServer::Configuration::UserBindServerConfigType
      UserBindServerConfig;

  public:
    UserBindServerImpl(
      Generics::ActiveObjectCallback* callback,
      Logging::Logger* logger,
      const UserBindServerConfig& user_bind_server_config)
      throw(Exception);

    virtual
    AdServer::UserInfoSvcs::UserBindMapper::BindRequestInfo*
    get_bind_request(
      const char* id,
      const CORBACommons::TimestampInfo& timestamp)
      throw(AdServer::UserInfoSvcs::UserBindServer::NotReady,
        AdServer::UserInfoSvcs::UserBindServer::ChunkNotFound);

    virtual
    void
    add_bind_request(
      const char* id,
      const AdServer::UserInfoSvcs::UserBindServer::BindRequestInfo& bind_request,
      const CORBACommons::TimestampInfo& timestamp)
      throw(AdServer::UserInfoSvcs::UserBindServer::NotReady,
        AdServer::UserInfoSvcs::UserBindServer::ChunkNotFound);

    virtual
    AdServer::UserInfoSvcs::UserBindMapper::GetUserResponseInfo*
    get_user_id(
      const AdServer::UserInfoSvcs::UserBindMapper::GetUserRequestInfo&
        request_info)
      throw(AdServer::UserInfoSvcs::UserBindServer::NotReady,
        AdServer::UserInfoSvcs::UserBindServer::ChunkNotFound);

    virtual
    AdServer::UserInfoSvcs::UserBindMapper::AddUserResponseInfo*
    add_user_id(
      const AdServer::UserInfoSvcs::UserBindMapper::AddUserRequestInfo&
        request_info)
      throw(AdServer::UserInfoSvcs::UserBindServer::NotReady,
        AdServer::UserInfoSvcs::UserBindServer::ChunkNotFound);

    virtual
    AdServer::UserInfoSvcs::UserBindServer::Source*
    get_source()
      throw(AdServer::UserInfoSvcs::UserBindServer::NotReady);

    Logging::Logger*
    logger() throw();

    virtual void
    wait_object()
      throw (Generics::ActiveObject::Exception, eh::Exception);

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;

    typedef AdServer::Commons::AccessActiveObject<
      UserBindProcessor_var>
      UserBindProcessorHolder;

    typedef ReferenceCounting::SmartPtr<UserBindProcessorHolder>
      UserBindProcessorHolder_var;

    typedef AdServer::Commons::AccessActiveObject<
      BindRequestProcessor_var>
      BindRequestProcessorHolder;

    typedef ReferenceCounting::SmartPtr<BindRequestProcessorHolder>
      BindRequestProcessorHolder_var;

    class LoadUserBindTask: public Generics::TaskGoal
    {
    public:
      LoadUserBindTask(
        Generics::TaskRunner* task_runner,
        UserBindServerImpl* user_bind_server_impl)
        throw();

      virtual void execute() throw();

    protected:
      UserBindServerImpl* user_bind_server_impl_;
      bool reschedule_;
    };

    class LoadBindRequestTask: public Generics::TaskGoal
    {
    public:
      LoadBindRequestTask(
        Generics::TaskRunner* task_runner,
        UserBindServerImpl* user_bind_server_impl)
        throw();

      virtual void execute() throw();

    protected:
      UserBindServerImpl* user_bind_server_impl_;
      bool reschedule_;
    };

    class ClearUserBindExpiredTask: public Generics::TaskGoal
    {
    public:
      ClearUserBindExpiredTask(
        Generics::TaskRunner* task_runner,
        UserBindServerImpl* user_bind_server_impl,
        bool reschedule)
        throw();

      virtual void execute() throw();

    protected:
      UserBindServerImpl* user_bind_server_impl_;
      bool reschedule_;
    };

    class ClearBindRequestExpiredTask: public Generics::TaskGoal
    {
    public:
      ClearBindRequestExpiredTask(
        Generics::TaskRunner* task_runner,
        UserBindServerImpl* user_bind_server_impl,
        bool reschedule)
        throw();

      virtual void execute() throw();

    protected:
      UserBindServerImpl* user_bind_server_impl_;
      bool reschedule_;
    };

    class DumpUserBindTask: public Generics::TaskGoal
    {
    public:
      DumpUserBindTask(
        Generics::TaskRunner* task_runner,
        UserBindServerImpl* user_bind_server_impl)
        throw();

      virtual void
      execute() throw();

    protected:
      UserBindServerImpl* user_bind_server_impl_;
    };

  protected:
    virtual
    ~UserBindServerImpl() throw() {};

    void
    load_user_bind_() throw();

    void
    load_bind_request_() throw();

    void
    clear_user_bind_expired_(bool reschedule) throw();

    void
    clear_bind_request_expired_(bool reschedule) throw();

    void
    dump_user_bind_() throw();

  private:
    Generics::ActiveObjectCallback_var callback_;
    Logging::Logger_var logger_;
    Generics::Planner_var scheduler_;
    Generics::TaskRunner_var task_runner_;

    const UserBindServerConfig user_bind_server_config_;
    UserBindContainer::ChunkPathMap chunks_;

    UserBindProcessorHolder_var user_bind_container_;
    BindRequestProcessorHolder_var bind_request_container_;

    Commons::UserIdBlackList user_id_black_list_;
  };

  typedef ReferenceCounting::SmartPtr<UserBindServerImpl>
    UserBindServerImpl_var;

} /* UserInfoSvcs */
} /* AdServer */

namespace AdServer
{
namespace UserInfoSvcs
{
  inline
  Logging::Logger*
  UserBindServerImpl::logger() throw()
  {
    return logger_;
  }
}
}

#endif /*USERINFOSVCS_USERBINDSERVERIMPL_HPP*/
