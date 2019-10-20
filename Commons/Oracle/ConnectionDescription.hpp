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

#ifndef ORACLE_CONNECTIONDESCRIPTION_HPP
#define ORACLE_CONNECTIONDESCRIPTION_HPP

#include <string>
#include <Generics/Time.hpp>

namespace AdServer
{
namespace Commons
{
  namespace Oracle
  {
    struct ConnectionDescription
    {
      ConnectionDescription();

      ConnectionDescription(
        const char* user_name_val,
        const char* password_val,
        const char* db_val,
        const char* schema_val = "",
        const Generics::Time* timeout = 0);

      std::string user_name;
      std::string password;
      std::string db;
      // isn't required, will be used default user schema if not defined
      std::string schema;
      Generics::Time statement_timeout;
    };
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
    ConnectionDescription::ConnectionDescription()
    {}

    inline
    ConnectionDescription::ConnectionDescription(
      const char* user_name_val,
      const char* password_val,
      const char* db_val,
      const char* schema_val,
      const Generics::Time* statement_timeout_val)
      : user_name(user_name_val),
        password(password_val),
        db(db_val),
        schema(schema_val),
        statement_timeout(
          statement_timeout_val ? *statement_timeout_val : Generics::Time::ZERO)
    {}
  }
}
}

#endif /*ORACLE_CONNECTIONDESCRIPTION_HPP*/
