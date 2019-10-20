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

#ifndef ADSERVER_COMMONS_VALUEGUARD_HPP
#define ADSERVER_COMMONS_VALUEGUARD_HPP

#include <ReferenceCounting/SmartPtr.hpp>
#include <Sync/SyncPolicy.hpp>

namespace AdServer
{
  namespace Commons
  {
    template <typename Type,
      typename SyncPolicyType = Sync::Policy::PosixThreadRW>
    class ValueGuard : private Generics::Uncopyable
    {
    public:
      typedef Type ValueType;

      template <typename... Args>
      explicit
      ValueGuard(Args... args) throw (eh::Exception);

      ValueGuard&
      operator =(Type&& new_value) throw (eh::Exception);

      template <typename T>
      ValueGuard&
      operator =(T&& new_value) throw (eh::Exception);

      Type
      get() const throw (eh::Exception);

    private:
      mutable typename SyncPolicyType::Mutex lock_;
      Type value_;
    };
  }
}

namespace AdServer
{
  namespace Commons
  {
    template <typename Type, typename SyncPolicyType>
    template <typename... Args>
    ValueGuard<Type, SyncPolicyType>::ValueGuard(Args... args)
      throw (eh::Exception)
      : value_(std::forward<Args>(args)...)
    {
    }

    template <typename Type, typename SyncPolicyType>
    ValueGuard<Type, SyncPolicyType>&
    ValueGuard<Type, SyncPolicyType>::operator =(Type&& new_value)
      throw (eh::Exception)
    {
      typename SyncPolicyType::WriteGuard guard(lock_);
      value_.swap(new_value);
      return *this;
    }

    template <typename Type, typename SyncPolicyType>
    template <typename T>
    ValueGuard<Type, SyncPolicyType>&
    ValueGuard<Type, SyncPolicyType>::operator =(T&& new_value)
      throw (eh::Exception)
    {
      typename SyncPolicyType::WriteGuard guard(lock_);
      value_ = std::forward<T>(new_value);
      return *this;
    }

    template <typename Type, typename SyncPolicyType>
    Type
    ValueGuard<Type, SyncPolicyType>::get() const throw (eh::Exception)
    {
      typename SyncPolicyType::ReadGuard guard(lock_);
      return value_;
    }
  }
}

#endif /*ADSERVER_COMMONS_VALUEGUARD_HPP*/
