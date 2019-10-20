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

#ifndef PROFILEMAP_MEMINDEXPROFILEMAP_HPP
#define PROFILEMAP_MEMINDEXPROFILEMAP_HPP

#include <set>
#include <map>
#include <memory>

#include <eh/Exception.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/MemBuf.hpp>
#include <Commons/LockMap.hpp>

#include <ProfilingCommons/PlainStorage/BaseLayer.hpp>
#include <ProfilingCommons/PlainStorage/DefaultSyncIndexStrategy.hpp>

#include "ProfileMap.hpp"

namespace AdServer
{
namespace ProfilingCommons
{
  template<
    typename KeyType,
    typename KeyAccessorType,
    typename BlockIndexType,
    typename BlockIndexSerializerType =
      PlainStorage::DefaultWriteBlockIndexAccessor<BlockIndexType> >
  struct DefaultMapTraits
  {
    /* key sync strategy */
    typedef KeyAccessorType KeySerializer;
    typedef PlainStorage::DefaultSyncIndexStrategy<
      KeyType,
      KeySerializer,
      BlockIndexType,
      BlockIndexSerializerType>
      SyncIndexStrategy;
    typedef BlockIndexType BlockIndex;
    typedef BlockIndexSerializerType BlockIndexSerializer;
  };

  /** MapBase */
  template<
    typename KeyType,
    typename BlockIndexType,
    typename MapTraitsType = DefaultMapTraits<
      KeyType,
      PlainStorage::DefaultWriteIndexAccessor<KeyType>,
      BlockIndexType>,
    template<typename, typename> class ContainerType = std::map>
  class MapBase:
    protected MapTraitsType::SyncIndexStrategy::IndexLoadCallback,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    typedef MapBase<KeyType, BlockIndexType, MapTraitsType, ContainerType>
      ThisType;
    typedef typename MapTraitsType::SyncIndexStrategy SyncIndexStrategy;

  public:
    class InterruptCallback: public virtual ReferenceCounting::Interface
    {
    public:
      virtual bool
      corrupted_record(ThisType* map, const KeyType& key, BlockIndexType index)
        throw () = 0;

    protected:
      virtual
      ~InterruptCallback() throw ()
      {}
    };

  public:
    MapBase(const char* file,
      InterruptCallback* interrupt = 0) throw(eh::Exception);

    MapBase(PlainStorage::WriteBaseLayer<BlockIndexType>* write_layer,
      PlainStorage::BlockAllocator<BlockIndexType>* allocator,
      InterruptCallback* interrupt = 0) throw (eh::Exception);

    MapBase(PlainStorage::WriteAllocateBaseLayer<BlockIndexType>* write_allocate_layer,
      InterruptCallback* interrupt = 0)
      throw (eh::Exception);

    virtual
    ~MapBase() throw();

    void
    load(
      PlainStorage::WriteBaseLayer<BlockIndexType>* write_layer,
      PlainStorage::BlockAllocator<BlockIndexType>* allocator,
      InterruptCallback* interrupt = 0)
      throw (eh::Exception);

    void
    close() throw(eh::Exception);

    bool
    check_block(const KeyType& key) const throw(Exception);

    PlainStorage::ReadBlock_var
    get_read_block(const KeyType& key) throw(Exception);

    void
    get_write_block(
      PlainStorage::WriteBlock_var& res,
      const KeyType& key,
      bool create_if_not_found = true)
      throw(Exception);

    bool
    remove_block(const KeyType& key) throw(Exception);

    unsigned long
    size() const throw();

    unsigned long
    area_size() const throw();

    bool
    empty() const throw();

    void
    copy_keys(std::list<KeyType>& keys)
      throw (eh::Exception);

  private:
    typedef std::unique_ptr<SyncIndexStrategy> SyncIndexStrategyPtr;

    struct ValueType
    {
      ValueType() {}
      
      ValueType(const BlockIndexType& index_val,
        const typename SyncIndexStrategy::KeyAddition& key_add_val)
        throw()
        : index(index_val),
          key_add(key_add_val)
      {}
      
      BlockIndexType index;
      typename SyncIndexStrategy::KeyAddition key_add;
    };

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;

    typedef ContainerType<KeyType, ValueType> IndexContainer;

  protected:
    virtual void
    load(
      const KeyType& key,
      const typename SyncIndexStrategy::KeyAddition& key_add,
      const BlockIndexType& block_ref)
      throw(eh::Exception);

    ValueType
    allocate_(PlainStorage::WriteBlock_var& block, const KeyType& key)
      throw(Exception);

  protected:
    mutable SyncPolicy::Mutex index_container_lock_;
    IndexContainer index_container_;

  private:
    ReferenceCounting::SmartPtr<PlainStorage::WriteBaseLayer<BlockIndexType> >
      write_layer_;
    ReferenceCounting::SmartPtr<PlainStorage::BlockAllocator<BlockIndexType> >
      block_allocator_;

    /// Synch with file strategy
    SyncIndexStrategyPtr sync_index_strategy_;

    std::set<BlockIndexType> block_index_check_;
  };

  /** MemIndexProfileMap */
  template<
    typename KeyType,
    typename BlockIndexType,
    typename MapTraitsType = DefaultMapTraits<
      KeyType,
      PlainStorage::DefaultWriteIndexAccessor<KeyType>,
      BlockIndexType>,
    template<typename, typename> class ContainerType = AdServer::Commons::Map2Args>
  class MemIndexProfileMap:
    protected virtual MapBase<KeyType, BlockIndexType, MapTraitsType, ContainerType>,
    public virtual ProfileMap<KeyType>,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    DECLARE_EXCEPTION(Exception, typename ProfileMap<KeyType>::Exception);

    typedef MapBase<KeyType, BlockIndexType, MapTraitsType, ContainerType>
      MapBaseType;
    typedef MemIndexProfileMap<KeyType, BlockIndexType, MapTraitsType, ContainerType>
      ThisType;

  public:
    typedef typename MapBaseType::InterruptCallback InterruptCallback;

  public:
    MemIndexProfileMap(
      const char* file,
      InterruptCallback* interrupt = 0)
      throw (eh::Exception);

    MemIndexProfileMap(
      PlainStorage::WriteAllocateBaseLayer<BlockIndexType>* write_allocate_layer,
      InterruptCallback* interrupt = 0)
      throw (eh::Exception);

    MemIndexProfileMap(
      PlainStorage::WriteBaseLayer<BlockIndexType>* write_layer,
      PlainStorage::BlockAllocator<BlockIndexType>* allocator,
      InterruptCallback* interrupt = 0) throw (eh::Exception);

    void load(
      PlainStorage::WriteBaseLayer<BlockIndexType>* write_layer,
      PlainStorage::BlockAllocator<BlockIndexType>* allocator,
      InterruptCallback* interrupt = 0)
      throw (eh::Exception);

    void close() throw(eh::Exception);

    virtual
    bool
    check_profile(const KeyType& key) const throw(Exception);

    virtual
    Generics::ConstSmartMemBuf_var
    get_profile(
      const KeyType& key,
      Generics::Time* last_access_time = 0)
      throw(Exception);

    virtual void
    save_profile(
      const KeyType& key,
      const Generics::ConstSmartMemBuf* mem_buf,
      const Generics::Time& now = Generics::Time::get_time_of_day(),
      OperationPriority op_priority = OP_RUNTIME)
      throw(Exception);
    
    virtual bool
    remove_profile(
      const KeyType& key,
      OperationPriority op_priority = OP_RUNTIME)
      throw(Exception);

    virtual unsigned long size() const throw();

    virtual unsigned long area_size() const throw();

    bool empty() const throw();

    void copy_keys(typename ProfileMap<KeyType>::KeyList& keys)
      throw(Exception);

    std::unique_ptr<KeyType> next_key(const KeyType* key)
      throw();

  private:
    virtual ~MemIndexProfileMap() throw();
  };
}
}

#include "MemIndexProfileMap.tpp"

#endif // PROFILEMAP_MEMINDEXPROFILEMAP_HPP
