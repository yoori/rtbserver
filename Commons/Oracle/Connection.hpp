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

#ifndef ORACLE_CONNECTION_HPP
#define ORACLE_CONNECTION_HPP

#include <Sync/PosixLock.hpp>

#include "OraException.hpp"
#include "InnerEntities.hpp"
#include "Statement.hpp"
#include "ConnectionDescription.hpp"

namespace AdServer
{
namespace Commons
{
  namespace Oracle
  {
    class Environment;
    typedef ReferenceCounting::SmartPtr<Environment> Environment_var;

    class Connection;

    struct ConnectionOwner:
      public virtual ReferenceCounting::Interface
    {
      // set owner (destroy controller)
      virtual void
      own_connection(Connection* conn) throw(NonActive) = 0;

      // connection pre destroy hook
      virtual bool
      destroy_connection(Connection* conn) throw() = 0;

      // connection post destroy hook
      virtual void
      connection_destroyed() throw() = 0;
    };

    typedef ReferenceCounting::SmartPtr<ConnectionOwner>
      ConnectionOwner_var;

    /** Connection */
    class Connection: public ReferenceCounting::AtomicImpl
    {
    public:
      typedef AdServer::Commons::Oracle::Exception Exception;
      typedef AdServer::Commons::Oracle::SqlException SqlException;
      typedef AdServer::Commons::Oracle::NotSupported NotSupported;

      Statement_var
      create_statement(const char* query)
        throw(Exception, SqlException, ConnectionError, NotSupported);

      void commit() throw(SqlException, ConnectionError, NotSupported);

      void rollback() throw(SqlException, ConnectionError, NotSupported);

      void terminate() throw();

      void cancel() throw(SqlException, ConnectionError);

      void free_cache() throw ();

    protected:
      friend class ConnectionOwner;
      friend class Environment;
      friend class SwitchableEnvironment;
      friend class StandartConnectionPool;
      friend class SwitchableConnectionPool;
      friend class Statement;
      friend class ResultSet;
      friend class SqlStream;
      friend class ParamArrayHolder;
      friend class ParamStreamHolder;
      friend class OCIObjectFillCache;

    protected:
      Connection(
        Environment* environment,
        ConnectionOwner* connection_owner,
        const ConnectionDescription& conn)
        throw(SqlException, ConnectionError);

      virtual ~Connection() throw();

      bool is_terminated_() throw();

      void execute_(const char* sql) throw(SqlException);

      // set destroy notify listener (notify old owner about destroy)
      void owner_(ConnectionOwner*) throw();

      virtual
      void
      delete_this_() const throw ();

    protected:
      Connection() throw();

    protected:
      Environment_var environment_;
      ConnectionOwner_var connection_owner_;
      Generics::Time timeout_;
      Sync::PosixMutex execute_lock_;

#ifdef _USE_OCCI
      OCIHandlePtr<OCIError, OCI_HTYPE_ERROR> error_handle_;

      OCIHandlePtr<OCIServer, OCI_HTYPE_SERVER> server_handle_;
      OCIHandlePtr<OCISession, OCI_HTYPE_SESSION> session_handle_;
      OCIHandlePtr<OCISvcCtx, OCI_HTYPE_SVCCTX> svc_context_handle_;
#endif

      bool broken_;
      volatile _Atomic_word terminated_;
    };

    typedef ReferenceCounting::SmartPtr<Connection> Connection_var;
  }
}
}

namespace AdServer
{
namespace Commons
{
  namespace Oracle
  {
    inline
    bool
    Connection::is_terminated_() throw()
    {
      return terminated_;
    }
  }
}
}

#endif /*ORACLE_CONNECTION_HPP*/
