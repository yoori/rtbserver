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

#ifndef ORACLE_EXCEPTION_HPP
#define ORACLE_EXCEPTION_HPP

#ifdef _USE_OCCI
#include <oci.h>
#endif

#include <eh/Exception.hpp>
#include <Generics/Time.hpp>

namespace
{
  const char EC_CONN_CREATE_FAILED[] = "connection create failed. ";
  const char CONN_TERMINATED_ERROR[] =
    ": Can't perform operation due to termination of the connection.";
  const char ORACLE_NOT_SUPPORTED[] =
    "Oci methods are not supported in current compilation. "
    "Please recompile with -D_USE_OCCI flag.";
}

namespace AdServer
{
namespace Commons
{
  namespace Oracle
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(NotSupported, Exception);
    DECLARE_EXCEPTION(SqlException, Exception);
    DECLARE_EXCEPTION(ConnectionError, Exception);
    DECLARE_EXCEPTION(TimedOut, ConnectionError);
    DECLARE_EXCEPTION(InvalidValue, Exception);
    DECLARE_EXCEPTION(NonActive, Exception);

#ifdef _USE_OCCI
    void oci_error_text(
      std::ostream& err,
      const char* fun,
      const char* oci_op,
      long status,
      OCIError* oci_error_handler = 0,
      const char* query = 0)
      throw();
        
    void throw_oci_error(
      const char* fun,
      const char* oci_op,
      long status,
      OCIError* oci_error_handler = 0,
      const char* query = 0)
      throw(SqlException);
#else
    void throw_oci_error(
      const char* fun,
      const char* oci_op,
      long status,
      void* oci_error_handler = 0,
      const char* query = 0)
      throw(SqlException);
#endif
      
    void throw_type_error(
      const char* fun,
      unsigned long pos,
      unsigned long type,
      bool null);

    void throw_timeout_error(
      const char* fun,
      const char* oci_op,
      const Generics::Time& passed_time,
      const Generics::Time& timeout)
      throw(TimedOut);
  }
}
}

#endif /*ORACLE_EXCEPTION_HPP*/
