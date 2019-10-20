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


#include "DbConnectionFactory.hpp"

namespace AdServer {
namespace LogProcessing {

void
PostgresConnectionFactoryImpl::initialize(const char* conn)
{
  if (initialized_)
  {
    return;
  }

  if (!env_.in() || !conn_pool_.in())
  {
    env_ = new Commons::Postgres::Environment(conn);
    conn_pool_ = env_->create_connection_pool();
  }
  initialized_ = true;
}

Commons::Postgres::Connection_var
PostgresConnectionFactoryImpl::get_connection()
{
  if (!initialized_ || !conn_pool_.in())
  {
    throw DbConnectionFactory::Exception(
            "AdServer::LogProcessing::PostgresConnectionFactoryImpl::"
            "get_connection(): Not initialized"
          );
  }

  Commons::Postgres::Connection_var conn;
  try
  {
    conn = conn_pool_->get_connection();
  }
  catch (const Commons::Postgres::NotActive&)
  {
    throw NotActive(""); // FIXME
  }
  return conn;
}

void PostgresConnectionFactoryImpl::activate_object()
  throw(ActiveObject::AlreadyActive)
{
  if (conn_pool_.in())
  {
    conn_pool_->activate_object();
  }
}

void PostgresConnectionFactoryImpl::deactivate_object()
  throw()
{
  if (conn_pool_.in())
  {
    conn_pool_->deactivate_object();
  }
}

void PostgresConnectionFactoryImpl::wait_object()
  throw()
{
  if (conn_pool_.in())
  {
    conn_pool_->wait_object();
  }
}

bool PostgresConnectionFactoryImpl::active()
  throw()
{
  return conn_pool_.in() ? conn_pool_->active() : false;
}

} // namespace LogProcessing
} // namespace AdServer

