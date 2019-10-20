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

#include <Sync/PosixLock.hpp>
#include <Generics/ArrayAutoPtr.hpp>

#include "Statement.hpp"
#include "Connection.hpp"
#include "Environment.hpp"
#include "TimeoutControl.hpp"


#ifdef _USE_OCCI
#  include "StatementImpl.cpp"
#else // !defined _USE_OCCI
#  include "StatementDummyImpl.cpp"
#endif

namespace AdServer {
namespace Commons {
namespace Oracle
{
  /** Statement */
  Statement::~Statement() throw()
  {}

  void
  Statement::execute(const Generics::Time* timeout)
    throw(SqlException, NotSupported, TimedOut)
  {
    execute_(timeout);
  }

  ResultSet_var
  Statement::execute_query(
    const Generics::Time* timeout,
    unsigned long fetch_size)
    throw(Exception, SqlException, TimedOut, NotSupported)
  {
    return execute_query_(timeout, fetch_size);
  }

  bool
  Statement::is_terminated_() throw()
  {
    return connection_->is_terminated_();
  }

  void Statement::check_terminated_(const char* fun)
    throw (NotSupported)
  {
    if(connection_->is_terminated_())
    {
      Stream::Error error;
      error << fun << CONN_TERMINATED_ERROR;
      throw NotSupported(error);
    }
  }

  const Generics::Time* Statement::use_timeout_(
    const Generics::Time* timeout) const
    throw()
  {
    if(timeout)
    {
      return timeout;
    }

    if(connection_->timeout_ != Generics::Time::ZERO)
    {
      return &connection_->timeout_;
    }

    if(connection_->environment_->timeout_ !=
       Generics::Time::ZERO)
    {
      return &connection_->environment_->timeout_;
    }

    return 0;
  }
  
  /** SqlStream */
  void SqlStream::set_int(int val) throw(SqlException)
  {
    set_long(val);
  }

  void SqlStream::set_uint(unsigned int val) throw(SqlException)
  {
    set_ulong(val);
  }
} /*Oracle*/
} /*Commons*/
} /*AdServer*/

