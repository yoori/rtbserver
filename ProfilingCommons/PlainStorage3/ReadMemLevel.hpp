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

#ifndef READMEMPLEVEL_HPP
#define READMEMPLEVEL_HPP

#include <Sync/SyncPolicy.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>

#include "BaseLevel.hpp"

namespace AdServer
{
namespace ProfilingCommons
{
  template<typename KeyType>
  class MemLevelHolder
  {
  public:
    struct __attribute__ ((__packed__)) ProfileHolderPOD
    {
      uint32_t access_time;
      unsigned char operation;
      unsigned long next_size; // sum size in next layers
    };

    struct ProfileHolder: public ProfileHolderPOD
    {
      Generics::ConstSmartMemBuf_var mem_buf;
    };

    typedef std::map<KeyType, ProfileHolder> ProfileHolderMap;

  public:
    MemLevelHolder()
      throw();

    CheckProfileResult
    check_profile_i(const KeyType& key) const
      throw(typename ReadBaseLevel<KeyType>::Exception);

    GetProfileResult
    get_profile_i(const KeyType& key) const
      throw(typename ReadBaseLevel<KeyType>::Exception);

    unsigned long
    size_i() const
      throw();

    unsigned long
    area_size_i() const
      throw();

    unsigned long
    merge_free_size_i() const
      throw();

    Generics::Time
    min_access_time_i() const
      throw();

    bool
    get_first_i(
      KeyType& key,
      Generics::ConstSmartMemBuf_var& mem_buf) const
      throw();

    static unsigned long
    eval_area_size_(const ProfileHolder& holder)
      throw();

  public:
    ProfileHolderMap profiles_;
    unsigned long size_;
    uint64_t area_size_;
    uint64_t merge_free_size_;
    Generics::Time min_access_time_;
  };

  template<typename KeyType, typename KeySerializerType>
  class RWMemLevel;

  template<typename KeyType, typename KeySerializerType>
  class ReadFileLevel;

  //
  // ReadMemLevel
  //
  template<typename KeyType>
  class ReadMemLevel:
    public ReadBaseLevel<KeyType>,
    public ReferenceCounting::AtomicImpl,
    public MemLevelHolder<KeyType>
  {
    /*
    template<typename KeyType2, typename KeySerializerType>
    friend class RWMemLevel<KeyType2, KeySerializerType>;
    */
  public:
    class KeyIteratorImpl:
      public ReadBaseLevel<KeyType>::KeyIterator,
      public ReferenceCounting::AtomicImpl
    {
    public:
      KeyIteratorImpl(const ReadMemLevel<KeyType>* read_mem_level)
        throw();

      virtual bool
      get_next(
        KeyType& key,
        ProfileOperation& operation,
        Generics::Time& access_time)
        throw();

    private:
      const ReferenceCounting::ConstPtr<ReadMemLevel<KeyType> > read_mem_level_;
      typename MemLevelHolder<KeyType>::ProfileHolderMap::
        const_iterator profiles_it_;
    };

    class IteratorImpl:
      public ReadBaseLevel<KeyType>::Iterator,
      public ReferenceCounting::AtomicImpl
    {
    public:
      IteratorImpl(const ReadMemLevel<KeyType>* read_mem_level)
        throw();

      virtual bool
      get_next(
        KeyType& key,
        ProfileOperation& operation,
        Generics::Time& access_time)
        throw();

      virtual Generics::ConstSmartMemBuf_var
      get_profile()
        throw();

    private:
      const ReferenceCounting::ConstPtr<ReadMemLevel<KeyType> > read_mem_level_;
      typename MemLevelHolder<KeyType>::ProfileHolderMap::
        const_iterator profiles_it_;
    };

  public:
    virtual CheckProfileResult
    check_profile(const KeyType& key) const
      throw(typename ReadBaseLevel<KeyType>::Exception);

    virtual GetProfileResult
    get_profile(const KeyType& key) const
      throw(typename ReadBaseLevel<KeyType>::Exception);

    virtual typename ReadBaseLevel<KeyType>::KeyIterator_var
    get_key_iterator() const
      throw();

    virtual typename ReadBaseLevel<KeyType>::Iterator_var
    get_iterator(unsigned long read_buffer_size) const
      throw();

    virtual unsigned long
    size() const
      throw();

    virtual unsigned long
    area_size() const
      throw();

    virtual unsigned long
    merge_free_size() const
      throw();

    virtual Generics::Time
    min_access_time() const
      throw();

  protected:
    virtual ~ReadMemLevel() throw()
    {}
  };
}
}

#include "ReadMemLevel.tpp"

#endif /*READMEMLEVEL_HPP*/
