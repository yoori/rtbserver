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

#ifndef POSTGRES_DECLARATION_HPP
#define POSTGRES_DECLARATION_HPP

#include <endian.h>

#define BINARY_RECIVING_DATA 1
 /*
  * * Interpretation of high bits.
  * */

#define NUMERIC_SIGN_MASK 0xC000
#define NUMERIC_POS 0x0000
#define NUMERIC_NEG 0x4000
#define NUMERIC_SHORT 0x8000
#define NUMERIC_NAN 0xC000
#define NBASE 10000
#define DEC_DIGITS 4 

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(ConnectionError, Exception);
    DECLARE_EXCEPTION(SqlException, Exception);
    DECLARE_EXCEPTION(NotSupported, Exception);
    DECLARE_EXCEPTION(NotActive, Exception);

    enum DATA_FORMATS
    {
      TEXT_FORMAT = 0,
      BINARY_FORMAT = 1,
      AUTO
    };

    /*OID from postrges headers */
    enum OIDS
    {
      CHAROID = 18,
      INT8OID = 20,
      INT2OID = 21,
      INT4OID = 23,
      TEXTOID = 25,
      OIDOID = 26,
      BPCHAROID = 1042,
      VARCHAROID = 1043,
      DATEOID = 1082,
      TIMEOID = 1083,
      TIMESTAMPOID = 1114,
      NUMERICOID = 1700
    };
  }
}
}

#endif //POSTGRES_DECLARATION_HPP

