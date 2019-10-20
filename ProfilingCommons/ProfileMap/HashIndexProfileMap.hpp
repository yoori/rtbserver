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

#ifndef PROFILEMAP_HASHINDEXPROFILEMAP_HPP
#define PROFILEMAP_HASHINDEXPROFILEMAP_HPP

#include <Generics/HashTableAdapters.hpp>

#include <Commons/LockMap.hpp>
#include <Commons/AtomicInt.hpp>
#include "MemIndexProfileMap.hpp"
#include "ProfileMap.hpp"

namespace AdServer
{
namespace ProfilingCommons
{
  template<
    typename KeyType,
    typename BlockIndexType,
    typename MapTraitsType = DefaultMapTraits<
      KeyType,
      PlainStorage::DefaultWriteIndexAccessor<KeyType>,
      BlockIndexType,
      PlainStorage::DefaultWriteBlockIndexAccessor<BlockIndexType> > >
  class HashIndexProfileMap:
    public virtual ProfileMap<KeyType>,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, typename ProfileMap<KeyType>::Exception);

  protected:
    typedef HashIndexProfileMap<KeyType, BlockIndexType, MapTraitsType> ThisType;
    typedef typename MapTraitsType::BlockIndexSerializer BlockIndexAccessor;
    typedef typename MapTraitsType::KeySerializer KeyAccessor;
    typedef Sync::Policy::PosixThread SyncPolicy;

  public:
    HashIndexProfileMap(const char* file) throw (eh::Exception);

    HashIndexProfileMap(
      PlainStorage::WriteBaseLayer<BlockIndexType>* write_layer,
      PlainStorage::BlockAllocator<BlockIndexType>* allocator) throw (eh::Exception);

    HashIndexProfileMap(
      PlainStorage::WriteAllocateBaseLayer<BlockIndexType>* write_allocate_layer)
      throw (eh::Exception);

    virtual
    ~HashIndexProfileMap() throw();

    void
    close() throw(eh::Exception);

    void
    load(
      PlainStorage::WriteBaseLayer<BlockIndexType>* write_layer,
      PlainStorage::BlockAllocator<BlockIndexType>* allocator)
      throw (eh::Exception);

    virtual
    bool
    check_profile(const KeyType& key) const throw(Exception);

    virtual
    Generics::ConstSmartMemBuf_var
    get_profile(
      const KeyType& key,
      Generics::Time*)
      throw(Exception);

    virtual void
    save_profile(
      const KeyType& key,
      const Generics::ConstSmartMemBuf* mem_buf,
      const Generics::Time& now,
      OperationPriority op_priority = OP_RUNTIME)
      throw(Exception);

    virtual bool
    remove_profile(
      const KeyType& key,
      OperationPriority op_priority = OP_RUNTIME)
      throw(Exception);

    virtual unsigned long
    size() const throw();

    virtual unsigned long
    area_size() const throw();

    virtual void
    copy_keys(typename ProfileMap<KeyType>::KeyList& keys)
      throw (Exception);

  private:
    typedef Generics::NumericHashAdapter<unsigned long> Hash;

    struct HashAccessor
    {
      PlainStorage::SizeType size(const Hash&) const
      {
        return 4;
      }

      void
      load(const void* buf, PlainStorage::SizeType size, Hash& out) const
        throw (typename ProfileMap<KeyType>::CorruptedRecord)
      {
        if (size != sizeof(u_int32_t))
        {
          Stream::Error ostr;
          ostr << "HashAccessor::load(): Try load incorrect size=" << size;
          throw typename ProfileMap<KeyType>::CorruptedRecord(ostr);
        }
        out = Hash(*static_cast<const u_int32_t*>(buf));
      }

      PlainStorage::SizeType
      save(const Hash& hash, void* buf, PlainStorage::SizeType size) const
        throw (typename ProfileMap<KeyType>::CorruptedRecord)
      {
        if (size != sizeof(u_int32_t))
        {
          Stream::Error ostr;
          ostr << "HashAccessor::save(): Try save incorrect size=" << size;
          throw typename ProfileMap<KeyType>::CorruptedRecord(ostr);
        }
        *static_cast<u_int32_t*>(buf) = hash.value();
        return 4;
      }
    };

    typedef PlainStorage::KeyBlockAdapter<
      KeyType, KeyAccessor, BlockIndexType, BlockIndexAccessor>
      KeyBlock;

    typedef AdServer::Commons::StrictLockMap<
      Hash,
      Sync::Policy::PosixThreadRW,
      AdServer::Commons::Hash2Args>
      KeyBlockLockMap;

    typedef MapBase<
      Hash,
      BlockIndexType,
      DefaultMapTraits<
        Hash,
        HashAccessor,
        typename MapTraitsType::BlockIndex,
        typename MapTraitsType::BlockIndexSerializer>,
      AdServer::Commons::Hash2Args>
      KeyMap;

  private:
    ReferenceCounting::SmartPtr<PlainStorage::WriteBaseLayer<BlockIndexType> >
      write_layer_;
    ReferenceCounting::SmartPtr<PlainStorage::BlockAllocator<BlockIndexType> >
      block_allocator_;

    mutable KeyBlockLockMap keys_block_lock_map_;
    mutable ReferenceCounting::SmartPtr<KeyMap> keys_block_map_;
  };
}
}

#include "HashIndexProfileMap.tpp"

#endif // PROFILEMAP_HASHINDEXPROFILEMAP_HPP
