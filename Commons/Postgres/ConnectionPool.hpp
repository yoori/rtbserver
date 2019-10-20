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

#ifndef POSTGRES_CONNECTION_POOL_HPP
#define POSTGRES_CONNECTION_POOL_HPP

#include<deque>
#include<Generics/ActiveObject.hpp>
#include<Sync/PosixLock.hpp>
#include<Commons/CountActiveObject.hpp>
#include<Commons/Postgres/Connection.hpp>
#include<Commons/Postgres/Environment.hpp>
#include<Commons/Postgres/ResultSet.hpp>

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    class ConnectionPool:
      public CountActiveObject,
      protected ConnectionOwner,
      protected Generics::RemovableActiveObject
    {
    public:
      DECLARE_EXCEPTION(MaxConnectionsReached, Postgres::Exception);

      ConnectionPool(
        Environment* env,
        Generics::ActiveObjectChildRemover* child_remover,
        unsigned int max_connections)
        throw ();

      virtual
      void
      deactivate_object() throw ();

      Connection_var
      get_connection(bool wait = true)
      throw (Postgres::Exception,
             Postgres::NotActive,
             MaxConnectionsReached);

      void bad_connection(Connection* conn) throw ();

    protected:

      // connection pre destroy hook
      virtual bool
      destroy_connection(Connection* conn) throw();

      // connection post destroy hook
      virtual void
      connection_destroyed() throw();

      Connection_var create_connection_i_()
        throw (ConnectionError, eh::Exception);

      Connection_var get_free_connection_i_()
        throw (ConnectionError, eh::Exception);

      virtual
      ~ConnectionPool() throw () {};
    private:
      typedef std::deque<Connection_var> ConnectionCont;
      Environment* env_;
      const int max_connections_;
      ConnectionCont free_connections_;
      mutable Sync::Condition free_connection_cond_;
    };
  }
}
}
#endif //POSTGRES_CONNECTION_POOL_HPP

