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

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <eh/Errno.hpp>

#include <deque>

#include <Frontends/FrontendCommons/FCGI.hpp>
#include "FrontendsPool.hpp"

#include "Acceptor.hpp"

namespace AdServer
{
namespace Frontends
{
  namespace Aspect
  {
    const char WORKER[] = "FCGI::Worker";
  }
  
  const Generics::Time WorkerStatsObject::STATS_TIMEOUT(5l);  // 5 seconds

  // WorkerStatsObject impl
  WorkerStatsObject::WorkerStatsObject(
    Logging::Logger* logger,
    Generics::ActiveObjectCallback* callback)
    throw (eh::Exception)
    : Commons::DelegateActiveObject(callback, 1),
      logger_(ReferenceCounting::add_ref(logger)),
      workers_count_(0),
      workers_inerval_max_(0),
      workers_max_(0)
  {}

  void
  WorkerStatsObject::work_() throw()
  {
    while(active())
    {
      Stream::Error ostr;
      {
        WorkerStatsSyncPolicy::ReadGuard lock(worker_stats_lock_);
        ostr << "Worker stats: current: " << workers_count_ <<
          ", max: : " << workers_max_ << ", last " << STATS_TIMEOUT.tv_sec << " seconds max: " <<
          workers_inerval_max_;
      }

      {
        WorkerStatsSyncPolicy::WriteGuard lock(worker_stats_lock_);
        workers_inerval_max_ = 0;
      }

      logger_->info(ostr.str(), Aspect::WORKER);
      
      {
        WorkerStatsSyncPolicy::WriteGuard lock(worker_cond_lock_);
        workers_condition_.timed_wait(worker_cond_lock_, &STATS_TIMEOUT, true);
      }
    }
  }

  void
  WorkerStatsObject::terminate_() throw()
  {
    WorkerStatsSyncPolicy::WriteGuard lock(worker_cond_lock_);
    workers_condition_.broadcast();
  }

  void
  WorkerStatsObject::incr_workers()
  {
    WorkerStatsSyncPolicy::WriteGuard lock(worker_stats_lock_);
    workers_count_++;
    if (workers_count_ > workers_inerval_max_)
    {
      workers_inerval_max_ = workers_count_;
    }

    if (workers_count_ > workers_max_)
    {
      workers_max_ = workers_count_;
    }
  }

  void
  WorkerStatsObject::dec_workers()
  {
    WorkerStatsSyncPolicy::WriteGuard lock(worker_stats_lock_);
    workers_count_--;
  }
 
  WorkerStatsObject::~WorkerStatsObject() throw()
  {}

  // State
  struct Acceptor::State: public ReferenceCounting::AtomicImpl
  {
    typedef Sync::Policy::PosixThread WorkersQueueSyncPolicy;
    typedef Sync::Policy::PosixThread JoinQueueSyncPolicy;
    typedef std::list<Worker*>::iterator WorkerId;

    WorkersQueueSyncPolicy::Mutex workers_lock;
    Sync::Conditional workers_empty;
    std::list<Worker*> workers;

    JoinQueueSyncPolicy::Mutex join_queue_lock;
    Sync::Conditional join_queue_empty;
    std::deque<pthread_t> join_queue;

    State(WorkerStatsObject* workers_stats);

    WorkerId
    reg_worker(Worker* w) throw();

    void
    unreg_worker(WorkerId id, pthread_t thread) throw();

  protected:
    virtual ~State() throw()
    {};

  private:
    WorkerStatsObject_var worker_stats_;
  };

  // Worker
  class Acceptor::Worker
  {
  public:
    Worker(
      Logging::Logger* logger,
      FrontendCommons::FrontendInterface* frontend,
      State* state,
      int sock)
      throw();

    virtual
    ~Worker() throw();

    void
    start();

    void
    stop();

  protected:
    static void*
    thread_proc(void* ctx);

    void
    work_() throw ();

    Logging::Logger*
    logger() throw();

  private:
    typedef Sync::PosixGuard Guard;

  private:
    static const int BUF_SIZE = 1024 * 1024; // 1 Mb
    Logging::Logger_var logger_;
    FrontendCommons::Frontend_var frontend_;
    State_var state_;

    int sock_;
    pthread_t thread_;
    volatile bool active_;
    State::WorkerId id_;
    char rbuf_[BUF_SIZE];
    Sync::PosixMutex rpos_lock_;
    char* rpos_;
    char wbuf_[BUF_SIZE];
  };

  // Acceptor::AcceptActiveObject
  class Acceptor::AcceptActiveObject: public Commons::DelegateActiveObject
  {
  public:
    AcceptActiveObject(
      Generics::ActiveObjectCallback* callback,
      Logging::Logger* logger,
      State* state,
      FrontendCommons::FrontendInterface* frontend,
      const char* bind_address,
      int backlog)
      throw (eh::Exception);

  protected:
    virtual
    ~AcceptActiveObject() throw();

    virtual void
    work_() throw ();

    virtual void
    terminate_() throw();

  private:
    Generics::ActiveObjectCallback_var callback_;
    Logging::Logger_var logger_;

    State_var state_;
    FrontendCommons::Frontend_var frontend_;
    const std::string bind_address_;

    int accept_sock_;
    bool remove_sock_;
  };

  // Acceptor::JoinActiveObject
  class Acceptor::JoinActiveObject: public Commons::DelegateActiveObject
  {
  public:
    JoinActiveObject(
      Generics::ActiveObjectCallback* callback,
      State* state,
      unsigned long threads)
      throw (eh::Exception);

    void
    work_() throw();

    virtual void
    terminate_() throw();

  protected:
    virtual
    ~JoinActiveObject() throw();

  private:
    State_var state_;
  };

  // State implementation
  Acceptor::State::State(WorkerStatsObject* worker_stats)
    : worker_stats_(ReferenceCounting::add_ref(worker_stats))
  {
  }
  
  Acceptor::State::WorkerId
  Acceptor::State::reg_worker(Worker* w) throw()
  {
    worker_stats_->incr_workers();
    {
      WorkersQueueSyncPolicy::WriteGuard lock(workers_lock);
      return workers.insert(workers.end(), w);
    }
  }

  void
  Acceptor::State::unreg_worker(WorkerId id, pthread_t thread) throw()
  {
    if(thread)
    {
      JoinQueueSyncPolicy::WriteGuard lock(join_queue_lock);
      const bool join_queue_is_empty = join_queue.empty();
      join_queue.push_back(thread);
      if(join_queue_is_empty)
      {
        join_queue_empty.signal();
      }
      
      {
        WorkersQueueSyncPolicy::WriteGuard lock(workers_lock);
        workers.erase(id);
        
        if(workers.empty())
        {
          workers_empty.broadcast();
        }
      }
      worker_stats_->dec_workers();
    }
  }

  // AcceptActiveObject implementation
  Acceptor::AcceptActiveObject::AcceptActiveObject(
    Generics::ActiveObjectCallback* callback,
    Logging::Logger* logger,
    Acceptor::State* state,
    FrontendCommons::FrontendInterface* frontend,
    const char* bind_address,
    int backlog)
    throw (eh::Exception)
    : Commons::DelegateActiveObject(callback, 10, 128*1024),
      callback_(ReferenceCounting::add_ref(callback)),
      logger_(ReferenceCounting::add_ref(logger)),
      state_(ReferenceCounting::add_ref(state)),
      frontend_(ReferenceCounting::add_ref(frontend)),
      bind_address_(bind_address)
  {
    accept_sock_ = ::socket(AF_LOCAL, SOCK_STREAM, 0);

    if(accept_sock_ == -1)
    {
      eh::throw_errno_exception<Exception>(errno, FNE, "socket() failed");
    }

    sockaddr_un bind_addr;
    ::memset(&bind_addr, 0, sizeof(bind_addr));

    bind_addr.sun_family = AF_UNIX;
    ::strncpy(bind_addr.sun_path, bind_address, sizeof(bind_addr.sun_path) - 1);

    // check if socket file was not removed earlier
    struct stat bind_address_stat;
    if(::stat(bind_address, &bind_address_stat) == 0 &&
      S_ISSOCK(bind_address_stat.st_mode))
    {
      ::unlink(bind_address);
    }

    if(::bind(
      accept_sock_,
      (struct sockaddr *)&bind_addr,
      sizeof(bind_addr)) == -1)
    {
      eh::throw_errno_exception<Exception>(errno, FNE, "bind() failed");
    }

    remove_sock_ = true;

    if (::listen(accept_sock_, backlog) == -1)
    {
      eh::throw_errno_exception<Exception>(errno, FNE, "listen() failed");
    }
  }

  Acceptor::AcceptActiveObject::~AcceptActiveObject() throw()
  {
    if(remove_sock_)
    {
      ::unlink(bind_address_.c_str());
    }

    if (accept_sock_ != -1)
    {
      ::close(accept_sock_);
    }
  }

  void
  Acceptor::AcceptActiveObject::work_() throw()
  {
    while(active())
    {
      sockaddr_un peer_addr;
      socklen_t p_size = sizeof(peer_addr);
      int res = ::accept(accept_sock_, (struct sockaddr*)&peer_addr, &p_size);
      if (res == -1)
      {
        if (active())
        {
          char err_msg[256];
          eh::ErrnoHelper::compose_safe(err_msg, sizeof(err_msg), errno, FNE, "accept() failed");
          callback_->error(String::SubString(err_msg));
        }
        continue;
      }

      Worker* w(new Worker(logger_, frontend_, state_, res)); // reg_worker
      w->start();
    }
  }

  void
  Acceptor::AcceptActiveObject::terminate_() throw()
  {
    if(accept_sock_ != -1)
    {
      ::shutdown(accept_sock_, SHUT_RDWR);
      ::close(accept_sock_);
      accept_sock_ = -1;
    }
  }

  // Acceptor::JoinActiveObject
  Acceptor::JoinActiveObject::JoinActiveObject(
    Generics::ActiveObjectCallback* callback,
    Acceptor::State* state,
    unsigned long threads)
    throw (eh::Exception)
    : Commons::DelegateActiveObject(callback, threads, 128*1024),
      state_(ReferenceCounting::add_ref(state))
  {}

  Acceptor::JoinActiveObject::~JoinActiveObject() throw()
  {}

  void
  Acceptor::JoinActiveObject::work_() throw()
  {
    while(active())
    {
      std::deque<pthread_t> join_queue;

      {
        Acceptor::State::JoinQueueSyncPolicy::WriteGuard lock(state_->join_queue_lock);

        while(active() && state_->join_queue.empty())
        {
          state_->join_queue_empty.wait(state_->join_queue_lock);
        }

        join_queue.swap(state_->join_queue);
      }

      for(auto t_it = join_queue.begin(); t_it != join_queue.end(); ++t_it)
      {
        pthread_join(*t_it, 0);
      }

      join_queue.clear();
    }
  }

  void
  Acceptor::JoinActiveObject::terminate_() throw()
  {
    Acceptor::State::JoinQueueSyncPolicy::WriteGuard lock(state_->join_queue_lock);
    state_->join_queue_empty.broadcast();
  }

  // Worker implementation
  Acceptor::Worker::Worker(
    Logging::Logger* logger,
    FrontendCommons::FrontendInterface* frontend,
    Acceptor::State* state,
    int sock)
    throw()
    : logger_(ReferenceCounting::add_ref(logger)),
      frontend_(ReferenceCounting::add_ref(frontend)),
      state_(ReferenceCounting::add_ref(state)),
      sock_(sock),
      thread_(0),
      active_(true),
      id_(state_->reg_worker(this)),
      rpos_(rbuf_)
  {}

  Acceptor::Worker::~Worker() throw()
  {
    state_->unreg_worker(id_, thread_);
    if (sock_ != -1)
    {
      close(sock_);
    }
  }

  Logging::Logger*
  Acceptor::Worker::logger() throw()
  {
    return logger_;
  }

  void
  Acceptor::Worker::start()
  {
    pthread_attr_t attr;
    size_t size = 1024 * 1024;
    ::pthread_attr_init(&attr);
    ::pthread_attr_setstacksize(&attr, size);
    int res = ::pthread_create(&thread_, 0, thread_proc, this);
    if (res)
    {
      char err_msg[256];
      eh::ErrnoHelper::compose_safe(err_msg, sizeof(err_msg), res, FNE, "pthread_create() failed");
      logger()->error(String::SubString(err_msg), Aspect::WORKER);
      delete this;
    }
  }

  void
  Acceptor::Worker::stop()
  {
    active_ = false;

    {
      Guard lock(rpos_lock_);
      if (rpos_ != rbuf_)
      {
        return;
      }
    }

    ::shutdown(sock_, SHUT_RDWR);
    ::close(sock_);
    sock_ = -1;
  }

  void*
  Acceptor::Worker::thread_proc(void* ctx)
  {
    std::unique_ptr<Worker> w(static_cast<Worker*>(ctx));
    w->work_();
    return 0;
  }

  void
  Acceptor::Worker::work_() throw()
  {
    while (active_)
    {
      FCGI::HttpRequest request;

      {
        Guard lock(rpos_lock_);
        rpos_ = rbuf_;
      }

      size_t rsize = 0;

      do
      {
        ssize_t recv_res = recv(sock_, rpos_, sizeof(rbuf_) - rsize, 0);

        if (recv_res == -1)
        { // TODO: handle stop event and signal interrupt
          char err_msg[256];
          eh::ErrnoHelper::compose_safe(err_msg, sizeof(err_msg), errno, FNE, "recv() failed");
          logger()->error(String::SubString(err_msg), Aspect::WORKER);
          return;
        }

        if (recv_res == 0)
        { // TODO: handle closed conn
          logger()->info(String::SubString("connection closed"), Aspect::WORKER);
          return;
        }

        {
          Guard lock(rpos_lock_);
          rpos_ += recv_res;
        }

        rsize += recv_res;

        int parse_res = request.parse(rbuf_, rsize);
        if (parse_res == FCGI::PARSE_OK)
        {
          break;
        }

        switch(parse_res)
        {
        case FCGI::PARSE_NEED_MORE:
          logger()->warning(String::SubString("getting PARSE_NEED_MORE"), Aspect::WORKER);
          break;

        case FCGI::PARSE_INVALID_HEADER:
          logger()->info(String::SubString("invalid fcgi header"), Aspect::WORKER);
          return;
        case FCGI::PARSE_BEGIN_REQUEST_EXPECTED:
          logger()->info(String::SubString("begin request expected"), Aspect::WORKER);
          return;
        case FCGI::PARSE_INVALID_ID:
          logger()->info(String::SubString("invalid FCGI header id"), Aspect::WORKER);
          return;
        case FCGI::PARSE_FRAGMENTED_STDIN:
          logger()->info(String::SubString("fragmented stdin"), Aspect::WORKER);
          return;
        }

        if (rsize >= sizeof(rbuf_))
        {
          logger()->info(String::SubString("request too big"), Aspect::WORKER);
          return;
        }
      }
      while(true);

      FCGI::HttpResponse response(1, wbuf_, sizeof(wbuf_));

      // process
      int res = 0;
      try
      {
        res = frontend_->handle_request_noparams(request, response);
      }
      catch (const eh::Exception& e)
      {
        res = 500; //HTTP_INTERNAL_SERVER_ERROR
        Stream::Error ostr;
        ostr << "Can't handle request '" << request.uri() <<
          "': " << e.what();
      
        logger()->error(ostr.str(), Aspect::WORKER); 
      }
      
      if (res == 0)
      {
        res = 200; // OK
      }

      std::vector<String::SubString> buffers;
      size_t sendsize = response.end_response(buffers, res);

      std::vector<iovec> v(buffers.size());
      struct msghdr msg;
      memset(&msg, 0, sizeof(msg));

      size_t vec_i = 0;
      for(auto buf_it = buffers.begin(); buf_it != buffers.end(); ++buf_it, ++vec_i)
      {
        v[vec_i].iov_base = (void*)buf_it->data();
        v[vec_i].iov_len = buf_it->size();
      }

      //size_t orig_sendsize = sendsize;
      msg.msg_iov = v.data();
      msg.msg_iovlen = v.size();

      while(sendsize)
      {
        if(msg.msg_iovlen == 0)
        {
          assert(0);
        }

        ssize_t res = sendmsg(sock_, &msg, 0);
        if (res == -1)
        {
          /*
            char err_msg[1024];
            eh::ErrnoHelper::compose_safe(
            err_msg,
            sizeof(err_msg),
            errno,
            FNE,
            "sendmsg() failed on ",
            sendsize,
            " bytes from ",
            orig_sendsize);
            logger()->error(String::SubString(err_msg), Aspect::WORKER);
          */
          return;
        }

        while(res > 0)
        {
          if(msg.msg_iov[0].iov_len > 0)
          {
            size_t n = std::min((size_t)res, msg.msg_iov[0].iov_len);
            msg.msg_iov[0].iov_base = ((char*)msg.msg_iov[0].iov_base) + n;
            msg.msg_iov[0].iov_len -= n;
            res -= n;
            sendsize -= n;
          }

          // assert(res == 0 || msg.msg_iov[0].iov_len == 0);

          if (msg.msg_iov[0].iov_len == 0)
          {
            ++msg.msg_iov;
            --msg.msg_iovlen;
          }
        }
      }
    }
  }

  // Acceptor implementation
  Acceptor::Acceptor(
    Logging::Logger* logger,
    FrontendCommons::FrontendInterface* frontend,
    Generics::ActiveObjectCallback* callback,
    const char* bind_address,
    unsigned long backlog,
    unsigned long join_threads)
    throw (eh::Exception)
    : callback_(ReferenceCounting::add_ref(callback)),
      logger_(ReferenceCounting::add_ref(logger)),
      frontend_(ReferenceCounting::add_ref(frontend)),
      worker_stats_object_(new WorkerStatsObject(
        logger,
        callback)),
      state_(new Acceptor::State(worker_stats_object_.in()))
  {
    add_child_object(Generics::ActiveObject_var(
      new AcceptActiveObject(
        callback,
        logger,
        state_,
        frontend_,
        bind_address,
        backlog)));

    join_active_object_ = new JoinActiveObject(
      callback,
      state_,
      join_threads);
  }

  Acceptor::~Acceptor() throw()
  {}

  void
  Acceptor::activate_object()
    throw (Exception, eh::Exception)
  {
    Generics::CompositeActiveObject::activate_object();

    worker_stats_object_->activate_object();
    join_active_object_->activate_object();
  }

  void
  Acceptor::deactivate_object()
    throw (Exception, eh::Exception)
  {
    Generics::CompositeActiveObject::deactivate_object();
  }

  void
  Acceptor::wait_object()
    throw (Exception, eh::Exception)
  {
    Generics::CompositeActiveObject::wait_object();
    // accept stopped - workers_ can't be increased now

    {
      // send gracefull stop to workers
      State::WorkersQueueSyncPolicy::WriteGuard lock(state_->workers_lock);
      for(auto worker_it = state_->workers.begin();
        worker_it != state_->workers.end(); ++worker_it)
      {
        (*worker_it)->stop();
      }
    }

    join_active_object_->deactivate_object();

    {
      State::JoinQueueSyncPolicy::WriteGuard lock(state_->join_queue_lock);
      state_->join_queue_empty.broadcast();
    }

    join_active_object_->wait_object();

    // all threads stopped - clean join_queue_ without lock
    for(auto thread_it = state_->join_queue.begin();
      thread_it != state_->join_queue.end(); ++thread_it)
    {
      ::pthread_join(*thread_it, 0);
    }

    // Deactivate worker stats
    worker_stats_object_->deactivate_object();
    worker_stats_object_->wait_object();

    frontend_->shutdown();
  }

  FrontendCommons::FrontendInterface*
  Acceptor::handler() throw()
  {
    return frontend_.in();
  }

  Logging::Logger*
  Acceptor::logger() throw()
  {
    return logger_.in();
  }
}
}
