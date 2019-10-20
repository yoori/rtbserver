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

// @file PlainStorage/CacheDelegateLayer.tpp

#include <iostream>
#include <map>

namespace PlainStorage
{
  /**
   * WriteCacheLayer
   */
  template<typename NextIndexType>
  WriteCacheDelegateLayer<NextIndexType>::WriteCacheDelegateLayer(
    WriteBaseLayer<NextBlockIndex>* next_layer,
    BlockAllocator<NextBlockIndex>* allocator,
    WriteBlockCache<NextBlockIndex>* write_block_cache)
    throw(BaseException)
    : TransientPropertyReadLayerImpl<NextIndexType, NextIndexType>(next_layer),
      TransientPropertyWriteLayerImpl<NextIndexType, NextIndexType>(next_layer),
      allocator_(ReferenceCounting::add_ref(allocator)),
      write_block_cache_(ReferenceCounting::add_ref(write_block_cache))
  {}

  template<typename NextIndexType>
  WriteCacheDelegateLayer<NextIndexType>::~WriteCacheDelegateLayer()
    throw()
  {
    write_block_cache_->clean_owner(this);
  }

  template<typename NextIndexType>
  ReadBlock_var
  WriteCacheDelegateLayer<NextIndexType>::get_read_block(
    const NextBlockIndex& block_index)
    throw(BaseException, CorruptedRecord)
  {
    return get_write_block(block_index);
  }

  template<typename NextIndexType>
  WriteBlock_var
  WriteCacheDelegateLayer<NextIndexType>::get_write_block(
    const NextIndexType& block_index)
    throw(BaseException, CorruptedRecord)
  {
    WriteBlock_var block_to_free;

    {
      typename BlockResolveLockMap::WriteGuard block_lock(
        block_resolve_locks_.write_lock(block_index));

      WriteBlock_var res =
        write_block_cache_->get(this, block_index);

      if(res)
      {
        return res;
      }

      res = this->next_layer()->get_write_block(block_index);

      block_to_free = write_block_cache_->insert(
        this, block_index, res);

      return res;
    }
  }

  template<typename NextIndexType>
  unsigned long
  WriteCacheDelegateLayer<NextIndexType>::area_size() const throw()
  {
    return this->next_layer()->area_size();
  }

  template<typename NextIndexType>
  void
  WriteCacheDelegateLayer<NextIndexType>::allocate(
    typename BlockAllocator<NextIndexType>::AllocatedBlock& alloc_result)
    throw(BaseException)
  {
    allocator_->allocate(alloc_result);

    alloc_result.block = alloc_result.block;

    write_block_cache_->insert(
      this,
      alloc_result.block_index,
      alloc_result.block);
  }
} // namespace PlainStorage
