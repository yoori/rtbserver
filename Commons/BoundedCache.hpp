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

#ifndef BOUNDEDCACHE_HPP
#define BOUNDEDCACHE_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/BoundedMap.hpp>
#include <Commons/LockMap.hpp>

namespace AdServer
{
namespace Commons
{
  /* BoundedCacheConfiguration requirements:
   *   Exception
   *
   *   Holder
   *
   *   bool update_required(const KeyType& key, const Holder& val) throw()
   *   Holder update(const KeyType& key) throw(Exception)
   *   size_type size(const Holder&) const throw()
   *   ValueType adapt(const Holder&) const throw()
   */
  template<
    typename KeyType,
    typename ValueType,
    typename ConfigurationType,
    template <typename, typename> class MapType = ReferenceCounting::HashTable>
  class BoundedCache: public ReferenceCounting::AtomicImpl
  {
  public:
    BoundedCache(
      unsigned long bound,
      const Generics::Time& timeout,
      const ConfigurationType& configuration = ConfigurationType())
      throw(eh::Exception);

    ValueType
    get(const KeyType& key, const char* service_index = 0) throw(
      typename ConfigurationType::Exception);

  private:
    typedef typename ConfigurationType::Holder Holder;

    class SizePolicy
    {
    public:
      explicit
      SizePolicy(const ConfigurationType& conf)
        : configuration_(conf)
      {}

      size_t operator()(const KeyType&, const Holder& holder) const throw()
      {
        return configuration_.size(holder);
      }

    private:
      const ConfigurationType& configuration_;
    };

    typedef Generics::BoundedMap<
      KeyType,
      Holder,
      SizePolicy,
      Sync::Policy::PosixThread,
      MapType<KeyType, typename Generics::BoundedMapTypes<KeyType, Holder>::Item> >
      ElementMap;

    typedef AdServer::Commons::LockMap<
      KeyType, Sync::Policy::PosixThread>
      KeyLockMap;

  private:
    virtual
    ~BoundedCache() throw ()
    {}

    typename ElementMap::iterator
    update_(
      typename ElementMap::iterator& it,
      const KeyType& key,
      const char* service_index)
      throw(typename ConfigurationType::Exception);

  private:
    ConfigurationType configuration_;
    ElementMap map_;
    KeyLockMap update_lock_map_;
  };
}
}

namespace AdServer
{
namespace Commons
{
  template<
    typename KeyType,
    typename ValueType,
    typename ConfigurationType,
    template <typename, typename> class MapType>
  BoundedCache<KeyType, ValueType, ConfigurationType, MapType>::
  BoundedCache(
    unsigned long bound,
    const Generics::Time& timeout,
    const ConfigurationType& configuration)
    throw(eh::Exception)
    : configuration_(configuration),
      map_(bound, timeout, SizePolicy(configuration_))
  {}

  template<
    typename KeyType,
    typename ValueType,
    typename ConfigurationType,
    template <typename, typename> class MapType>
  ValueType
  BoundedCache<KeyType, ValueType, ConfigurationType, MapType>::
  get(const KeyType& key, const char* service_index)
    throw(typename ConfigurationType::Exception)
  {
    typename ElementMap::iterator it = map_.find(key);
    if(it != map_.end())
    {
      if(configuration_.update_required(it->first, it->second))
      {
        it = update_(it, key, service_index);
      }
    }
    else
    {
      it = update_(it, key, service_index);
    }

    return configuration_.adapt(it->second);
  }

  template<
    typename KeyType,
    typename ValueType,
    typename ConfigurationType,
    template <typename, typename> class MapType>
  typename BoundedCache<KeyType, ValueType, ConfigurationType, MapType>::
    ElementMap::iterator
  BoundedCache<KeyType, ValueType, ConfigurationType, MapType>::
  update_(
    typename ElementMap::iterator& it,
    const KeyType& key,
    const char* service_index)
    throw (typename ConfigurationType::Exception)
  {
    typename KeyLockMap::WriteGuard lock = update_lock_map_.write_lock(key);
    if(it != map_.end())
    {
      map_.erase(it);
    }

    Holder h(configuration_.update(key, it != map_.end() ? &it->second: 0, service_index));
    return map_.insert(typename ElementMap::value_type(key, h)).first;
  }
}
}

#endif //BOUNDEDCACHE_HPP
