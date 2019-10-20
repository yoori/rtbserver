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

#include <Stream/MemoryStream.hpp>

#include "OraException.hpp"

namespace AdServer
{
namespace Commons
{
namespace Oracle
{
#ifdef _USE_OCCI
  void oci_error_text(
    std::ostream& error,
    const char* fun,
    const char* oci_op,
    long status,
    OCIError* oci_error_handler,
    const char* query)
    throw ()
  {
    error << fun << ": can't make " << oci_op << ": status = '";

    switch(status)
    {
    case OCI_NEED_DATA:
      error << "OCI_NEED_DATA";
      break;
    case OCI_NO_DATA:
      error << "OCI_NO_DATA";
      break;
    case OCI_INVALID_HANDLE:
      error << "OCI_INVALID_HANDLE";
      break;
    case OCI_STILL_EXECUTING:
      error << "OCI_STILL_EXECUTING";
      break;
    case OCI_CONTINUE:
      error << "OCI_CONTINUE";
      break;
    case OCI_SUCCESS_WITH_INFO:
      error << "OCI_SUCCESS_WITH_INFO";
      break;
    case OCI_ERROR:
      error << "OCI_ERROR";
      break;
    default:
      error << "UNKNOWN(" << status << ")";
      break;
    }

    error << "'";

    if(oci_error_handler &&
       (status == OCI_SUCCESS_WITH_INFO ||
        status == OCI_ERROR))
    {
      text err_buf[1024] = "";
      sb4 err_code = 0;
      ub4 msg_number = 1;

      while (OCIErrorGet(
        oci_error_handler,
        msg_number++,
        0,
        &err_code,
        err_buf,
        sizeof(err_buf),
        static_cast<ub4>(OCI_HTYPE_ERROR)) != OCI_NO_DATA)
      {
        error << ", message = '" << err_buf << "', error code = " << err_code;
      }
    }

    if(query)
    {
      error << ", sql = " << query;
    }
  }

  void throw_oci_error(
    const char* fun,
    const char* oci_op,
    long status,
    OCIError* oci_error_handler,
    const char* query)
    throw(SqlException)
  {
    Stream::Error error;
    oci_error_text(error, fun, oci_op, status, oci_error_handler, query);
    throw SqlException(error);
  }

  void throw_type_error(
    const char* fun,
    unsigned long pos,
    unsigned long type,
    bool null)
  {
    std::string oci_type_name;
    switch(type)
    {
    case SQLT_VNU:
      oci_type_name = "Number";
      break;
    case SQLT_ODT:
      oci_type_name = "Date";
      break;
    case SQLT_STR:
      oci_type_name = "String";
      break;
    default:
      oci_type_name = "Unknown";
    }

    Stream::Error error;
    if(!null)
    {
      error << fun << ": Can't use '" << oci_type_name << "' as source, ";
    }
    else
    {
      error << fun << ": Can't use null as source, ";
    }
    error << "pos = " << pos << ".";
    throw SqlException(error);
  }

#else
  void
  throw_oci_error(
    const char* /*fun*/,
    const char* /*oci_op*/,
    long /*status*/,
    void* /*oci_error_handler*/,
    const char* /*query*/,
    const char* /*comment*/)
    throw(SqlException)
  {}

  void throw_type_error(
    const char*,
    unsigned long,
    bool)
  {}

#endif

  void throw_timeout_error(
    const char* fun,
    const char* oci_op,
    const Generics::Time& passed_time,
    const Generics::Time& timeout)
    throw(TimedOut)
  {
    Stream::Error ostr;
    ostr << fun << "can't make " << oci_op << ": timeout expired, passed = " <<
      passed_time << ", timeout = " << timeout;
    throw TimedOut(ostr);
  }
}
}
}
