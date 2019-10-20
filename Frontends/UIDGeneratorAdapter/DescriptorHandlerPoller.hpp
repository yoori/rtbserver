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

#ifndef EPOLLER_HPP_
#define EPOLLER_HPP_

#include <unordered_map>

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include <Commons/AtomicInt.hpp>

namespace AdServer
{
  // DescriptorHandler
  class DescriptorHandler: public ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    enum StateChange
    {
      CONTINUE_HANDLE = 0,
      STOP_READ_HANDLE = 1,
      START_READ_HANDLE = 2,
      STOP_WRITE_HANDLE = 4,
      START_WRITE_HANDLE = 8,
      STOP_PROCESSING = 16,
      STOP_HANDLE = STOP_READ_HANDLE | STOP_WRITE_HANDLE
    };

  public:
    virtual int
    fd() const throw() = 0;

    // return false if need stop loop
    virtual unsigned long
    read() throw(Exception) = 0;

    // return false if need stop loop
    virtual unsigned long
    write() throw(Exception) = 0;

    virtual void
    stopped() throw()
    {};
  };

  typedef ReferenceCounting::SmartPtr<DescriptorHandler>
    DescriptorHandler_var;

  // DescriptorHandlerPoller
  class DescriptorHandlerPoller:
    public Generics::CompositeActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    class Proxy: public ReferenceCounting::AtomicImpl
    {
      friend class DescriptorHandlerPoller;

    public:
      bool
      add(DescriptorHandler* desc_handler)
        throw(Exception);

    protected:
      typedef Sync::Policy::PosixThreadRW SyncPolicy;

    protected:
      Proxy(DescriptorHandlerPoller* poller);

      virtual
      ~Proxy() throw();

      void
      detach_() throw();

      ReferenceCounting::SmartPtr<DescriptorHandlerPoller>
      lock_owner_() const throw();

    protected:
      mutable SyncPolicy::Mutex lock_;
      DescriptorHandlerPoller* owner_;
    };

    typedef ReferenceCounting::SmartPtr<Proxy> Proxy_var;

  public:
    DescriptorHandlerPoller(
      Generics::ActiveObjectCallback* callback,
      unsigned long threads,
      const Generics::Time& connection_clean_period = Generics::Time::ONE_SECOND)
      throw(Exception);

    void
    add(DescriptorHandler* desc_handler)
      throw(Exception);

    // stop all process calls
    void
    stop() throw();

    Proxy_var
    proxy() const throw();

    void
    deactivate_object()
      throw(Generics::ActiveObject::Exception, eh::Exception);

    void
    wait_object()
      throw(Generics::ActiveObject::Exception, eh::Exception);

    static void
    set_non_blocking(int fd)
      throw(Exception);

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;

    class StopPipeDescriptorHandler;

    typedef ReferenceCounting::SmartPtr<StopPipeDescriptorHandler>
      StopPipeDescriptorHandler_var;

    struct DescriptorHandlerHolder: public ReferenceCounting::AtomicImpl
    {
      DescriptorHandlerHolder(DescriptorHandler* descriptor_handler)
        throw();

      virtual
      ~DescriptorHandlerHolder() throw()
      {
        descriptor_handler = DescriptorHandler_var();
        destroyed = true;
      }

      DescriptorHandler_var descriptor_handler;
      bool handle_read;
      bool handle_write;
      Algs::AtomicInt handle_in_progress;
      Algs::AtomicInt handling_finished;
      bool destroyed;
    };

    typedef ReferenceCounting::SmartPtr<DescriptorHandlerHolder>
      DescriptorHandlerHolder_var;

    typedef std::unordered_map<DescriptorHandler*, DescriptorHandlerHolder_var>
      DescriptorHandlerMap;

    class DescriptorHandlerCleaner;

    typedef ReferenceCounting::SmartPtr<DescriptorHandlerCleaner>
      DescriptorHandlerCleaner_var;

  protected:
    virtual
    ~DescriptorHandlerPoller() throw();

    void
    process_(unsigned long thread_i)
      throw();

    bool
    handle_(
      unsigned long thread_i,
      DescriptorHandlerHolder* descriptor_handler_holder,
      uint32_t events);

    // epoll helpers
    void
    epoll_del_fd_(int fd)
      throw(Exception);

    void
    epoll_rearm_fd_(
      unsigned long thread_i,
      DescriptorHandlerHolder* descriptor_handler_holder)
      const throw(Exception);

    bool
    apply_state_modify_(
      unsigned long thread_i,
      DescriptorHandlerHolder* descriptor_handler_holder,
      unsigned long state_modify)
      const throw();

  protected:
    Generics::ActiveObjectCallback_var callback_;
    const int connection_clean_period_;

    Proxy_var proxy_;
    int epoll_fd_;
    StopPipeDescriptorHandler_var stop_handler_;
    DescriptorHandlerCleaner_var descriptor_handler_cleaner_;

    SyncPolicy::Mutex handlers_lock_;
    DescriptorHandlerMap handlers_;
  };

  typedef ReferenceCounting::SmartPtr<DescriptorHandlerPoller>
    DescriptorHandlerPoller_var;
}

#endif /*EPOLLER_HPP_*/
