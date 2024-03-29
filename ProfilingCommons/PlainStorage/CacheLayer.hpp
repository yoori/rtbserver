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

/**
 * @file PlainStorage/CacheLayer.hpp
 * CacheLayer's - simple keep last used blocks
 */

#ifndef _PLAINSTORAGE_CACHELAYER_HPP_
#define _PLAINSTORAGE_CACHELAYER_HPP_

#include <functional>

#include <Sync/SyncPolicy.hpp>
#include <ReferenceCounting/Map.hpp>
#include <Generics/BoundedMap.hpp>

#include "BaseLayer.hpp"
#include "TransientPropertyLayerImpl.hpp"

namespace PlainStorage
{
  /**
   * ReadCacheLayer Stores read blocks requested by the latter
   */
  template<typename NextIndexType>
  class ReadCacheLayer:
    public virtual TransientPropertyReadLayerImpl<NextIndexType, NextIndexType>
  {
  public:
    typedef NextIndexType NextBlockIndex;
    typedef NextIndexType BlockIndex;

    /**
     * Init table for blocks
     * @param next_layer The reference to next layer
     * @param cache_size Size of table that caching write blocks
     */
    ReadCacheLayer(
      ReadBaseLayer<NextBlockIndex>* next_layer,
      unsigned long cache_size) throw(BaseException);

    /**
     * Empty virtual destructor
     */
    virtual ~ReadCacheLayer() throw();

    /**
     * Search in the container of opened blocks,
     * if found - returns from it. No - creating a read block and put it into
     * the cache.
     * @param index The index block that we need
     * @return The smart pointer to requested block
     */
    virtual
    ReadBlock_var
    get_read_block(const BlockIndex& index)
      throw(BaseException, CorruptedRecord);

    virtual
    unsigned long area_size() const throw();

  protected:
    typedef Sync::Policy::PosixThread SyncPolicy;
    typedef
      Generics::BoundedMap<
        BlockIndex,
        ReadBlock_var,
        Generics::DefaultSizePolicy<BlockIndex, ReadBlock_var>,
        Sync::Policy::PosixThread,
        ReferenceCounting::Map<BlockIndex,
          typename Generics::BoundedMapTypes<BlockIndex, ReadBlock_var>::
            Item> >
      ReadBlockTable;

    mutable SyncPolicy::Mutex lock_;
    /// Store cached blocks
    ReadBlockTable block_table_;
  };

  /**
   * WriteCacheLayer Stores write blocks requested by the latter
   */
  template<typename NextIndexType>
  class WriteCacheLayer:
    public virtual TransientPropertyWriteLayerImpl<NextIndexType, NextIndexType>,
    public virtual BlockAllocator<NextIndexType>
  {
  public:
    typedef NextIndexType NextBlockIndex;
    typedef NextIndexType BlockIndex;

    /**
     * Init table for blocks
     * @param next_layer The reference to next layer
     * @param allocator Need to be able to allocate the block and immediately
     * put it in the cache
     * @param max_cache_size The maximum size of table that caching write
     * blocks
     */
    WriteCacheLayer(
      WriteBaseLayer<NextBlockIndex>* next_layer,
      BlockAllocator<NextBlockIndex>* allocator,
      unsigned long max_cache_size) throw(BaseException);

    /**
     * Empty virtual destructor
     */
    virtual ~WriteCacheLayer() throw();

    /**
     * Delegates to get_write_block
     */
    virtual
    ReadBlock_var
    get_read_block(const BlockIndex& index)
      throw(BaseException, CorruptedRecord);

    /**
     * Search in the container of opened blocks,
     * if found - returns from it. No - creating a write block and put it into
     * the cache.
     * @param index The index block that we need
     * @return The smart pointer to requested block
     */
    virtual
    WriteBlock_var
    get_write_block(const BlockIndex& index)
      throw(BaseException, CorruptedRecord);

    virtual
    unsigned long area_size() const throw();

    /**
     * Blocks allocation service.
     * Allocated block is inserted into the cache immediately.
     */
    virtual
    void allocate(
      typename BlockAllocator<NextIndexType>::AllocatedBlock& alloc_result)
      throw(BaseException);

  protected:
    typedef
      Generics::BoundedMap<
        BlockIndex,
        WriteBlock_var,
        Generics::DefaultSizePolicy<BlockIndex, WriteBlock_var>,
        Sync::Policy::PosixThread,
        ReferenceCounting::Map<BlockIndex,
          typename Generics::BoundedMapTypes<BlockIndex, WriteBlock_var>::
            Item> >
      WriteBlockTable;

    ReferenceCounting::SmartPtr<BlockAllocator<NextIndexType> > allocator_;

    /// Store cached blocks
    WriteBlockTable block_table_;
  };
}

#include "CacheLayer.tpp"

#endif // _PLAINSTORAGE_CACHELAYER_HPP_
