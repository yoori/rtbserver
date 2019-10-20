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

#include<libpq-fe.h>
#include<ReferenceCounting/Deque.hpp>
#include "Environment.hpp"
#include "Connection.hpp"
#include "ConnectionPool.hpp"


namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    Environment::Environment(const char* conninfo) throw()
      : conn_info_(conninfo),
        count_(new CountActiveObject),
        children_(new Generics::CompositeSetActiveObject)
    {
      add_child_object(count_);
      add_child_object(children_);
    }

    Connection_var
    Environment::create_connection(ConnectionOwner* owner)
      throw(ConnectionError, eh::Exception)
    {
      if(!owner)
      {
        owner = this;
      }
      Connection_var res = new Connection(conn_info_.c_str(), owner);
      count_->add_active_count(1, true);
      return res;
    }

    ConnectionPool_var
    Environment::create_connection_pool(unsigned long max_conn)
      throw(ConnectionError)
    {
      ConnectionPool_var res =
        new Commons::Postgres::ConnectionPool(this, children_, max_conn);
      children_->add_child_object(res);
      return res;
    }

    bool
    Environment::destroy_connection(Connection*)
      throw()
    {
      return true;
    }

    void
    Environment::connection_destroyed()
      throw()
    {
      count_->add_active_count(-1);
    }
  }
}
}
