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
#include <String/StringManip.hpp>

#include "TimeoutControl.hpp"
#include "ResultSet.hpp"
#include "Statement.hpp"
#include "Connection.hpp"
#include "Environment.hpp"

#ifdef _USE_OCCI
# include "ResultSetImpl.cpp"
#else
# include "ResultSetDummyImpl.cpp"
#endif

namespace AdServer {
namespace Commons {
namespace Oracle
{
  ResultSet::~ResultSet() throw()
  {}
  
  void ResultSet::check_terminated_(const char* fun) const
    throw (NotSupported)
  {
    if(statement_->is_terminated_())
    {
      Stream::Error error;
      error << fun << CONN_TERMINATED_ERROR;
      throw NotSupported(error);
    }
  }

  ResultSet::Column::~Column() throw()
  {
    clear_fetch_cells_();
  }

  Generics::Time ResultSet::get_date(unsigned int ind) const
    throw(Exception, SqlException, NotSupported)
  {
    return get_timestamp(ind);
  }
}
}
}
