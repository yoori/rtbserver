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

#ifndef POSTGRES_ENVIRONMENT_HPP
#define POSTGRES_ENVIRONMENT_HPP

#include<string>
#include<Generics/CompositeActiveObject.hpp>
#include<Commons/Postgres/Connection.hpp>
#include<Commons/CountActiveObject.hpp>
#include<Sync/SyncPolicy.hpp>

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    class ConnectionPool;

    typedef ReferenceCounting::SmartPtr<ConnectionPool>
      ConnectionPool_var;

    class Environment:
      public Generics::CompositeActiveObject,
      public virtual ReferenceCounting::AtomicImpl,
      protected ConnectionOwner
    {
    public:
      explicit
      Environment(const char* conninfo) throw();

      Connection_var
      create_connection(ConnectionOwner* owner = 0)
        throw (ConnectionError, eh::Exception);

      ConnectionPool_var
      create_connection_pool(unsigned long max_conn = 0)
        throw(ConnectionError);

      virtual bool
      destroy_connection(Connection*) throw();

      virtual void
      connection_destroyed() throw();

      friend class ConnectionPool;
    protected:
      virtual
      ~Environment() throw () = default;

    private:
      std::string conn_info_;

      CountActiveObject_var count_;
      Generics::CompositeSetActiveObject_var children_;
    };

    typedef ReferenceCounting::SmartPtr<Environment> Environment_var;
  }
}
}
#endif //POSTGRES_ENVIRONMENT_HPP
