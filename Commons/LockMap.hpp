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

#ifndef LOCKMAP_HPP
#define LOCKMAP_HPP

#include <vector>
#include <map>
#include <Generics/GnuHashTable.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <ReferenceCounting/HashTable.hpp>
#include <Sync/SyncPolicy.hpp>

namespace AdServer
{
namespace Commons
{
  template<typename KeyType, typename ValueType>
  class Map2Args: public std::map<KeyType, ValueType>
  {
  };

  template<typename KeyType, typename ValueType>
  class Hash2Args: public Generics::GnuHashTable<KeyType, ValueType>
  {
  };

  template<typename KeyType, typename ValueType>
  class RCHash2Args: public ReferenceCounting::HashTable<KeyType, ValueType>
  {
  };

  template<typename KeyType,
    typename SyncPolicyType = Sync::Policy::PosixThreadRW,
    template<typename, typename> class ContainerType = Map2Args>
  class StrictLockMap
  {
  private:
    typedef Sync::Policy::PosixThread SyncPolicy;

    class LockHolder: public ReferenceCounting::AtomicImpl
    {
    public:
      LockHolder(StrictLockMap& map_val, const KeyType& key_val) throw();

      StrictLockMap<KeyType, SyncPolicyType, ContainerType>& map;
      KeyType key;
      mutable typename SyncPolicyType::Mutex lock;

    protected:
      virtual ~LockHolder() throw() {}

      virtual bool
      remove_ref_no_delete_() const throw();
    };

    typedef ReferenceCounting::SmartPtr<LockHolder> LockHolder_var;

    template<typename GuardType>
    struct GuardHolder: public ReferenceCounting::AtomicImpl
    {
      GuardHolder(LockHolder* lock_holder_val)
        : lock_holder(ReferenceCounting::add_ref(lock_holder_val)),
          guard(lock_holder->lock)
      {}

      LockHolder_var lock_holder;
      GuardType guard;
    };

  public:
    typedef ReferenceCounting::SmartPtr<
      GuardHolder<typename SyncPolicyType::ReadGuard> >
      ReadGuard;
    typedef ReferenceCounting::SmartPtr<
      GuardHolder<typename SyncPolicyType::WriteGuard> >
      WriteGuard;
    
  public:
    ReadGuard read_lock(const KeyType& key) throw();

    WriteGuard write_lock(const KeyType& key) throw();

    typedef ContainerType<KeyType, LockHolder*> LockHolderMap;

  protected:
    template<typename GuardType>
    GuardType* get_(const KeyType& key) throw()
    {
      LockHolder_var holder;
      
      {
        SyncPolicy::WriteGuard guard(map_lock_);

        typename LockHolderMap::const_iterator it = map_.find(key);
        if(it != map_.end())
        {
          holder = ReferenceCounting::add_ref(it->second);
        }
        else
        {
          holder = new LockHolder(*this, key);
          map_.insert(std::make_pair(key, holder.in()));
        }
      }

      return new GuardType(holder);
    }

    void close_i_(const KeyType& key) throw();
    
  private:
    SyncPolicy::Mutex map_lock_;
    LockHolderMap map_;
  };

  template<typename KeyType,
    typename SyncPolicyType = Sync::Policy::PosixThreadRW>
  class NoAllocLockMap
  {
  private:
    template<typename GuardType>
    struct GuardHolder: public ReferenceCounting::AtomicImpl
    {
      GuardHolder(typename SyncPolicyType::Mutex& lock_val)
        : guard(lock_val)
      {}

      GuardType guard;
    };

  public:
    typedef ReferenceCounting::SmartPtr<
      GuardHolder<typename SyncPolicyType::ReadGuard> >
      ReadGuard;
    typedef ReferenceCounting::SmartPtr<
      GuardHolder<typename SyncPolicyType::WriteGuard> >
      WriteGuard;

    struct LockWrap: public SyncPolicyType::Mutex
    {
      LockWrap()
      {}

      LockWrap(const LockWrap&)
        : SyncPolicyType::Mutex()
      {}

      LockWrap&
      operator=(const LockWrap&)
      {
        return *this;
      }
    };

    typedef std::vector<LockWrap> LockArray;

  public:
    NoAllocLockMap(unsigned long size = 100) throw();

    ReadGuard
    read_lock(const KeyType& key) throw();

    WriteGuard
    write_lock(const KeyType& key) throw();

  protected:
    template<typename GuardType>
    GuardType* get_(const KeyType& key) throw()
    {
      unsigned long lock_i = key.hash() % locks_.size();
      return new GuardType(locks_[lock_i]);
    }

  private:
    LockArray locks_;
  };

  template<typename KeyType,
    typename SyncPolicyType = Sync::Policy::PosixThreadRW>
  class LockMap: public NoAllocLockMap<KeyType, SyncPolicyType>
  {
  public:
    LockMap(unsigned long size = 100) throw()
      : NoAllocLockMap<KeyType, SyncPolicyType>(size)
    {}
  };
}
}

namespace AdServer
{
namespace Commons
{
  // StrictStrictLockMap
  template<typename KeyType, typename SyncPolicyType,
    template<typename, typename> class ContainerType>
  StrictLockMap<KeyType, SyncPolicyType, ContainerType>::LockHolder::LockHolder(
    StrictLockMap<KeyType, SyncPolicyType, ContainerType>& map_val,
    const KeyType& key_val) throw()
    : map(map_val), key(key_val)
  {}

  template<typename KeyType, typename SyncPolicyType,
    template<typename, typename> class ContainerType>
  bool
  StrictLockMap<KeyType, SyncPolicyType, ContainerType>::LockHolder::remove_ref_no_delete_() const throw()
  {
    SyncPolicy::WriteGuard guard(map.map_lock_);
    if(ReferenceCounting::AtomicImpl::remove_ref_no_delete_())
    {
      map.close_i_(key);
      return true;
    }
    return false;
  }

  template<typename KeyType, typename SyncPolicyType,
    template<typename, typename> class ContainerType>
  typename StrictLockMap<KeyType, SyncPolicyType, ContainerType>::ReadGuard
  StrictLockMap<KeyType, SyncPolicyType, ContainerType>::read_lock(const KeyType& key)
    throw()
  {
    return ReadGuard(get_<
      GuardHolder<typename SyncPolicyType::ReadGuard> >(key));
  }

  template<typename KeyType, typename SyncPolicyType,
    template<typename, typename> class ContainerType>
  typename StrictLockMap<KeyType, SyncPolicyType, ContainerType>::WriteGuard
  StrictLockMap<KeyType, SyncPolicyType, ContainerType>::write_lock(const KeyType& key)
    throw()
  {
    return WriteGuard(get_<
      GuardHolder<typename SyncPolicyType::WriteGuard> >(key));
  }

  template<typename KeyType, typename SyncPolicyType,
    template<typename, typename> class ContainerType>
  void
  StrictLockMap<KeyType, SyncPolicyType, ContainerType>::close_i_(const KeyType& key) throw()
  {
    typename LockHolderMap::size_type erased_keys = map_.erase(key);
    (void)erased_keys;
    assert(erased_keys);
  }

  // NoAllocLockMap
  template<typename KeyType, typename SyncPolicyType>
  NoAllocLockMap<KeyType, SyncPolicyType>::NoAllocLockMap(
    unsigned long size)
    throw()
  {
    locks_.resize(size);
  }

  template<typename KeyType, typename SyncPolicyType>
  typename NoAllocLockMap<KeyType, SyncPolicyType>::ReadGuard
  NoAllocLockMap<KeyType, SyncPolicyType>::read_lock(
    const KeyType& key)
    throw()
  {
    return ReadGuard(get_<
      GuardHolder<typename SyncPolicyType::ReadGuard> >(key));
  }

  template<typename KeyType, typename SyncPolicyType>
  typename NoAllocLockMap<KeyType, SyncPolicyType>::WriteGuard
  NoAllocLockMap<KeyType, SyncPolicyType>::write_lock(
    const KeyType& key)
    throw()
  {
    return WriteGuard(get_<
      GuardHolder<typename SyncPolicyType::WriteGuard> >(key));
  }
}
}

#endif /*LOCKMAP_HPP*/
