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

#ifndef PROFILEMAP_PROFILEMAP_HPP
#define PROFILEMAP_PROFILEMAP_HPP

#include <list>
#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <Generics/Time.hpp>
#include <Generics/MemBuf.hpp>

namespace AdServer
{
namespace ProfilingCommons
{
  enum OperationPriority
  {
    OP_RUNTIME,
    OP_BACKGROUND
  };

  template<typename KeyType>
  struct ProfileMap: public virtual ReferenceCounting::Interface
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
    DECLARE_EXCEPTION(CorruptedRecord, Exception);

    typedef KeyType KeyTypeT;
    typedef std::list<KeyType> KeyList;

    virtual void
    wait_preconditions(const KeyType&, OperationPriority) const
      throw(Exception)
    {}

    virtual bool
    check_profile(const KeyType& key) const throw(Exception) = 0;

    virtual Generics::ConstSmartMemBuf_var
    get_profile(
      const KeyType& key,
      Generics::Time* last_access_time = 0)
      throw(Exception) = 0;

    virtual void
    save_profile(
      const KeyType& key,
      const Generics::ConstSmartMemBuf* mem_buf,
      const Generics::Time& now = Generics::Time::get_time_of_day(),
      OperationPriority op_priority = OP_RUNTIME)
      throw(Exception) = 0;

    virtual bool
    remove_profile(
      const KeyType& key,
      OperationPriority op_priority = OP_RUNTIME)
      throw(Exception) = 0;

    virtual void clear_expired(const Generics::Time& /*expire_time*/)
      throw(Exception)
    {
      throw Exception("clear_expired isn't supported");
    }

    virtual void copy_keys(KeyList& /*keys*/) throw(Exception)
    {
      throw Exception("copy_keys isn't supported");
    };

    virtual unsigned long size() const throw() = 0;

    virtual unsigned long area_size() const throw() = 0;
  };
}
}

#endif /*PROFILEMAP_PROFILEMAP_HPP*/
