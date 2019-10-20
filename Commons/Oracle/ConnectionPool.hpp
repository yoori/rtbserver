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

#ifndef ORACLE_CONNECTIONPOOL_HPP
#define ORACLE_CONNECTIONPOOL_HPP

#include <vector>
#include <Generics/CompositeActiveObject.hpp>
#include <Commons/CountActiveObject.hpp>

#include "OraException.hpp"
#include "Connection.hpp"

namespace AdServer
{
namespace Commons
{
namespace Oracle
{
  class ConnectionPool;

  //
  // PooledConnection
  //   extend Connection with ability to unlink it from ConnectionPool
  //
  class PooledConnection: public Connection
  {
  public:
    // remove connection from pool
    void unpool() throw();

  protected:
    friend class StandartConnectionPool;
    friend class SwitchableConnectionPool;

    PooledConnection(
      Environment* environment,
      ConnectionOwner* owner,
      const ConnectionDescription& conn)
      throw(SqlException, ConnectionError);

    virtual ~PooledConnection() throw() {};
  };

  typedef ReferenceCounting::SmartPtr<PooledConnection>
    PooledConnection_var;

  //
  // ConnectionPool
  //
  class ConnectionPool:
    public virtual ReferenceCounting::AtomicImpl,
    protected ConnectionOwner
  {
  public:
    DECLARE_EXCEPTION(MaxConnectionsReached, eh::DescriptiveException);

    virtual Connection_var
    get_connection(bool wait = true)
      throw(MaxConnectionsReached, NonActive);

    virtual unsigned long busy_connections() const throw() = 0;

    unsigned long open_connections() const throw();

    unsigned long max_connections() const throw();

  protected:
    ConnectionPool(
      Environment* environment,
      const ConnectionDescription& conn,
      unsigned long max_connections = 0)
      throw(SqlException, ConnectionError);

    virtual ~ConnectionPool() throw();

    unsigned long open_connections_i_() const throw();

    bool add_free_connection_(Connection* conn) throw();

    virtual bool
    add_free_connection_i_(Connection* conn) throw();

    // if check_connection_i_ returned create_connection = true
    // will be called one of create_connection_i_,
    // create_connection_canceled_i_ methods
    virtual Connection_var
    check_connection_i_(
      bool& create_connection,
      bool wait_free_connection)
      throw(MaxConnectionsReached, NonActive);

    virtual Connection_var
    create_connection_i_()
      throw(ConnectionError, NonActive) = 0;

    virtual void
    create_connection_canceled_i_() throw() = 0;

  protected:
    friend class PooledConnection;

    typedef std::vector<Connection_var> ConnectionList;

  protected:
    const ReferenceCounting::SmartPtr<Environment> environment_;
    const ConnectionDescription conn_descr_;
    const unsigned long max_connections_;

    mutable Sync::Condition free_connection_cond_;
    ConnectionList free_connections_;

    // serialize connections creation
    Sync::PosixMutex create_lock_;
  };

  typedef ReferenceCounting::SmartPtr<ConnectionPool>
    ConnectionPool_var;

  //
  // StandartConnectionPool
  //
  class StandartConnectionPool: public ConnectionPool
  {
  public:
    virtual unsigned long
    busy_connections() const throw();

  protected:
    friend class Environment;

  protected:
    StandartConnectionPool(
      Environment* environment,
      const ConnectionDescription& conn,
      unsigned long max_connections)
      throw(SqlException, ConnectionError);

    virtual ~StandartConnectionPool() throw();

  protected:
    virtual void
    own_connection(Connection* conn) throw(NonActive);

    virtual bool
    destroy_connection(Connection* conn) throw();

    virtual void
    connection_destroyed() throw();

    virtual Connection_var
    check_connection_i_(
      bool& create_connection,
      bool wait_free_connection)
      throw(MaxConnectionsReached, NonActive);

    virtual Connection_var
    create_connection_i_()
      throw(ConnectionError, NonActive);

    virtual void
    create_connection_canceled_i_() throw();

  private:
    Algs::AtomicInt busy_connections_;
  };

  //
  // SwitchableConnectionPool
  //
  class SwitchableConnectionPool:
    public ConnectionPool,
    public CountActiveObject,
    protected Generics::RemovableActiveObject
  {
  public:
    virtual void deactivate_object() throw();

    virtual unsigned long busy_connections() const throw();

  protected:
    friend class SwitchableEnvironment;

  protected:
    SwitchableConnectionPool(
      Environment* environment,
      Generics::ActiveObjectChildRemover* child_remover,
      const ConnectionDescription& conn,
      unsigned long max_connections = 0)
      throw(SqlException);

    virtual ~SwitchableConnectionPool() throw();

    // ConnectionOwner
    virtual void
    own_connection(Connection* conn) throw(NonActive);

    virtual bool
    destroy_connection(Connection* conn) throw();

    virtual void
    connection_destroyed() throw();

    virtual bool
    add_free_connection_i_(Connection* conn) throw();

    virtual Connection_var
    check_connection_i_(
      bool& create_connection,
      bool wait_free_connection)
      throw(MaxConnectionsReached, NonActive);

    virtual Connection_var
    create_connection_i_()
      throw(ConnectionError, NonActive);

    void
    create_connection_canceled_i_() throw();

    virtual
    void
    before_remove_child_() throw ();
  };

  typedef ReferenceCounting::SmartPtr<SwitchableConnectionPool>
    SwitchableConnectionPool_var;
}
}
}

namespace AdServer
{
namespace Commons
{
namespace Oracle
{
  //
  // ConnectionPool
  //
  inline
  ConnectionPool::~ConnectionPool() throw()
  {}

  inline
  unsigned long
  ConnectionPool::open_connections() const throw()
  {
    Sync::PosixGuard lock(free_connection_cond_);
    return open_connections_i_();
  }

  inline
  unsigned long
  ConnectionPool::max_connections() const throw()
  {
    return max_connections_;
  }

  inline
  unsigned long
  ConnectionPool::open_connections_i_() const throw()
  {
    return free_connections_.size() + busy_connections();
  }

  //
  // StandartConnectionPool
  //
  inline
  StandartConnectionPool::~StandartConnectionPool() throw()
  {}

  inline
  unsigned long
  StandartConnectionPool::busy_connections() const throw()
  {
    return busy_connections_;
  }
}
}
}

#endif /*ORACLE_CONNECTIONPOOL_HPP*/
