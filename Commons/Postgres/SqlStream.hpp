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

#ifndef POSTGRES_SQL_STREAM_HPP
#define POSTGRES_SQL_STREAM_HPP

#include<vector>
#include<sstream>
#include<Generics/Time.hpp>
#include<eh/Exception.hpp>
#include<ReferenceCounting/AtomicImpl.hpp>
#include<ReferenceCounting/SmartPtr.hpp>
#include<Commons/Postgres/Declarations.hpp>
#include<Commons/Postgres/Statement.hpp>

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    class SqlStream;

    class Object: public ReferenceCounting::AtomicImpl
    {
    public:
      virtual const char* getSQLTypeName() const throw(eh::Exception) = 0;

      virtual void writeSQL(SqlStream& stream)
        throw(eh::Exception, SqlException) = 0;

      virtual void readSQL(SqlStream& stream)
        throw(eh::Exception, SqlException) = 0;

    protected:
      virtual ~Object() throw();
    };

    class SqlStream: public ReferenceCounting::AtomicImpl
    {
    public:

      SqlStream() throw() : pos_(0) {};

      template<class TypeValue>
      void set_value(
        const TypeValue& value)
        throw (eh::Exception);

      void set_date(
        const Generics::Time& date)
        throw (eh::Exception);

      void set_time(
        const Generics::Time& time)
        throw (eh::Exception);

      void set_timestamp(
        const Generics::Time& timestamp)
        throw (eh::Exception);

      void set_string(const std::string& str)
        throw(eh::Exception);

      void set_null() throw(eh::Exception);

      template<typename DecimalType>
      void set_decimal(const DecimalType& val)
        throw(eh::Exception);

      const std::string str() throw();

    protected:
      virtual ~SqlStream() throw() {};
      
    private:
      void next_field_() throw ();

    private:
      std::ostringstream stream_;
      size_t pos_;
    };
    
    typedef ReferenceCounting::SmartPtr<SqlStream> SqlStream_var;
  }
}
}

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    template<class TypeValue>
    void SqlStream::set_value(
      const TypeValue& value)
      throw (eh::Exception)
    {
      next_field_();
      stream_ << value;
    }

    template<typename DecimalType>
    void SqlStream::set_decimal(const DecimalType& val)
      throw(eh::Exception)
    {
      set_value(val);
    }

    inline
    const std::string SqlStream::str() throw()
    {
      return stream_.str();
    }

    inline
    Object::~Object() throw()
    {}

  }
}
}
#endif //COMMONS_POSTGRES_SQL_STREAM_HPP

