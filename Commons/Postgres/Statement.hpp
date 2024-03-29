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

#ifndef POSTGRES_STATEMENT_HPP
#define POSTGRES_STATEMENT_HPP

#include<libpq-fe.h>
#include<vector>
#include<sstream>
#include<ReferenceCounting/AtomicImpl.hpp>
#include<ReferenceCounting/SmartPtr.hpp>
#include<Commons/Postgres/Declarations.hpp>
#include<Generics/Time.hpp>

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    class Connection;
    class Object;

    typedef ReferenceCounting::SmartPtr<Object> Object_var;
    typedef std::vector<Object_var> ObjectVector;

    class Statement: public ReferenceCounting::AtomicImpl
    {
    public:
      enum DataTypes
      {
        CHAR = 0,
        STRING,
        NUMBER,
        INT,
        UNSIGNED_INT,
        FLOAT,
        BLOB,
        DATE,
        TIMESTAMP
      };

      
      Statement(const char* query, const char* name = 0)
        throw ();

      /*
      void
      set_param_type(unsigned int param_index, DataTypes param_type) throw();

      void
      set_param_type(unsigned int param_index, const char* param_type) throw();
      */

      template<class ContainerType>
      void set_array(
        unsigned int param_num,
        const ContainerType& cont)
        throw (Exception);

      void set_array(
        unsigned int param_num,
        const ObjectVector& objs)
        throw (Exception);

      template<class TypeValue>
      void set_value(
        unsigned int param_num,
        const TypeValue& value)
        throw (Exception);

      void set_date(
        unsigned int param_num,
        const Generics::Time& date)
        throw (eh::Exception);

      void set_time(
        unsigned int param_num,
        const Generics::Time& time)
        throw (eh::Exception);

      void set_timestamp(
        unsigned int param_num,
        const Generics::Time& timestamp)
        throw (eh::Exception);

      const std::string& name() const throw ();

      const std::vector<std::string>& params() const throw ();

      void prepare_query(PGconn* conn, int count_params)
        throw (SqlException);

      const std::string& query() const throw ();
    protected:
      virtual ~Statement() throw () {};

      void add_param_(const std::string& str, unsigned int param_index)
        throw (eh::Exception);

    private:
      std::vector<std::string> params_;
      std::string name_;
      std::string query_;
    };

    typedef ReferenceCounting::SmartPtr<Statement> Statement_var;

  }
}
}

namespace AdServer
{
namespace Commons
{
  namespace Postgres
  {
    inline
    const std::string& Statement::name() const throw()
    {
      return name_;
    }

    inline
    const std::vector<std::string>& Statement::params() const throw ()
    {
      return params_;
    }

    inline
    const std::string& Statement::query() const throw ()
    {
      return query_;
    }

    inline
    void Statement::add_param_(const std::string& str, unsigned int param_index)
      throw (eh::Exception)
    {
      if(params_.size() <= param_index)
      {
        params_.resize(param_index + 1);
      }
      params_[param_index] = str;
    }

    template<class ContainerType>
    void Statement::set_array(
      unsigned int param_num,
      const ContainerType& cont)
      throw(Exception)
    {
      std::ostringstream ostr;
      ostr << '{';
      for(typename ContainerType::const_iterator it = cont.begin();
          it != cont.end(); it++)
      {
        if(it != cont.begin())
        {
          ostr << ", ";
        }
        ostr << *it;
      }
      ostr << '}';
      add_param_(ostr.str(), param_num - 1);
    }

    template<class TypeValue>
    void Statement::set_value(
      unsigned int param_num,
      const TypeValue& value)
      throw(Exception)
    {
      std::ostringstream ostr;
      ostr << value;
      add_param_(ostr.str(), param_num - 1);
    }

  }
}
}

#endif //POSTGRES_STATEMENT_HPP

