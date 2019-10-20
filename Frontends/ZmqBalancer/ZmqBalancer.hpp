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

#ifndef ZMQBALANCER_HPP_
#define ZMQBALANCER_HPP_

#include <memory>
#include <string>
#include <ext/atomicity.h>

#include <Generics/ActiveObject.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Generics/TaskRunner.hpp>
#include <eh/Exception.hpp>
#include <Sync/SyncPolicy.hpp>

#include <Commons/DelegateActiveObject.hpp>
#include <Commons/ProcessControlVarsImpl.hpp>
#include <Commons/zmq.hpp>

#include <xsd/Frontends/ZmqBalancerConfig.hpp>
#include "ZmqStreamStats.hpp"

namespace AdServer
{
class ZmqBalancer_ :
  public virtual Generics::CompositeActiveObject,
  public virtual AdServer::Commons::ProcessControlVarsLoggerImpl
{
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

public:
  static ReferenceCounting::QualPtr<ZmqBalancer_>
  create(char** argv) throw (eh::Exception);

  void
  main() throw();

  //
  // IDL:CORBACommons/IProcessControl/shutdown:1.0
  //
  virtual void
  shutdown(CORBA::Boolean wait_for_completion)
    throw();

private:
  ZmqBalancer_(const char* aspect) throw(eh::Exception);

  virtual
  ~ZmqBalancer_() throw()
  {}

private:
  typedef xsd::AdServer::Configuration::ZmqBalancerConfigType
    ZmqBalancerConfig;

  typedef xsd::AdServer::Configuration::ZmqSocketType
      SocketConfig;

  typedef std::unique_ptr<ZmqBalancerConfig> ZmqBalancerConfigPtr;

  typedef Sync::Policy::PosixThread SyncPolicy;

  class ZmqWorker : public AdServer::Commons::DelegateActiveObject
  {
  public:
    ZmqWorker(
      Generics::ActiveObjectCallback* callback,
      zmq::context_t& context,
      ZmqStreamStats* stats,
      const SocketConfig& config,
      unsigned long worker_number,
      unsigned long worker_threads,
      const char* address,
      const char* router_socketname);

  protected:
    virtual
    ~ZmqWorker() throw () = default;

    void
    work_() throw ();

  private:
    Generics::ActiveObjectCallback_var callback_;
    zmq::context_t& context_;
    ZmqStreamStats_var stats_;
    const SocketConfig& config_;
    unsigned long worker_number_;
    std::string address_;
    std::string router_socketname_;
  };

  typedef ReferenceCounting::AssertPtr<ZmqWorker>::Ptr ZmqWorker_var;
 
  class ZmqClient : public AdServer::Commons::DelegateActiveObject
  {
  public:
    ZmqClient(
      Generics::ActiveObjectCallback* callback,
      zmq::context_t& context,
      ZmqStreamStats* stats,
      const SocketConfig& config,
      unsigned long workers_count,
      const char* router_socketname);

  protected:
    virtual
    ~ZmqClient() throw () = default;

    void
    work_() throw ();

  private:
    Generics::ActiveObjectCallback_var callback_;
    zmq::context_t& context_;
    ZmqStreamStats_var stats_;
    const SocketConfig& config_;
    //zmq::socket_t broker_;
    //zmq::socket_t client_;
    unsigned long workers_count_;
    std::string router_socketname_;
  };

  typedef ReferenceCounting::AssertPtr<ZmqClient>::Ptr ZmqClient_var;

private:
  void
  read_config_(
    const char *filename,
    const char* argv0)
    throw(Exception, eh::Exception);

  void
  init_corba_() throw(Exception);

  void
  init_zeromq_() throw(Exception);

  void
  fill_stats_() throw();

private:
  const std::string ASPECT_;

  CORBACommons::CorbaServerAdapter_var corba_server_adapter_;
  CORBACommons::CorbaConfig corba_config_;
  ZmqBalancerConfigPtr config_;
  ZmqStreamStats_var stats_;

  std::unique_ptr<zmq::context_t> zmq_context_;
  Generics::TaskRunner_var task_runner_;
  Generics::Planner_var scheduler_;
  SyncPolicy::Mutex shutdown_lock_;

};

typedef ReferenceCounting::QualPtr<ZmqBalancer_> ZmqBalancer_var;

}

#endif /* ZMQBALANCER_HPP_ */
