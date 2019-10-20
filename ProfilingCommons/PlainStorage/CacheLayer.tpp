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

// @file PlainStorage/CacheLayer.tpp

#include "FileLayer.hpp"

#include <fstream>
#include <iostream>
#include <map>

namespace PlainStorage
{
  /**
   * ReadCacheLayer
   */
  template<typename NextIndexType>
  ReadCacheLayer<NextIndexType>::ReadCacheLayer(
    ReadBaseLayer<NextBlockIndex>* next_layer,
    SizeType max_cache_size)
    throw(BaseException)
    : TransientPropertyReadLayerImpl<NextIndexType, NextIndexType>(
        next_layer),
      block_table_(max_cache_size, Generics::Time(0))
  {}

  template<typename NextIndexType>
  ReadCacheLayer<NextIndexType>::~ReadCacheLayer() throw()
  {}

  template<typename NextIndexType>
  ReadBlock_var
  ReadCacheLayer<NextIndexType>::get_read_block(
    const NextIndexType& block_index)
    throw(BaseException, CorruptedRecord)
  {
    /* search for opened blocks */
    {
      SyncPolicy::ReadGuard lock(lock_);
      typename ReadBlockTable::const_iterator exist_it =
        block_table_.find(block_index);
      if(exist_it != block_table_.end())
      {
        return exist_it->second;
      }
    }

    SyncPolicy::WriteGuard lock(lock_);

    ReadBlock_var ret = this->next_layer()->get_read_block(block_index);

    block_table_.insert(std::make_pair(block_index, ret));

    return ret;
  }

  template<typename NextIndexType>
  unsigned long
  ReadCacheLayer<NextIndexType>::area_size() const
    throw()
  {
    return this->next_layer()->area_size();
  }

  /**
   * WriteCacheLayer
   */
  template<typename NextIndexType>
  WriteCacheLayer<NextIndexType>::WriteCacheLayer(
    WriteBaseLayer<NextBlockIndex>* next_layer,
    BlockAllocator<NextBlockIndex>* allocator,
    unsigned long max_cache_size)
    throw(BaseException)
    : TransientPropertyReadLayerImpl<NextIndexType, NextIndexType>(next_layer),
      TransientPropertyWriteLayerImpl<NextIndexType, NextIndexType>(next_layer),
      allocator_(ReferenceCounting::add_ref(allocator)),
      block_table_(max_cache_size, Generics::Time(0))
  {}

  template<typename NextIndexType>
  WriteCacheLayer<NextIndexType>::~WriteCacheLayer() throw()
  {}

  template<typename NextIndexType>
  ReadBlock_var
  WriteCacheLayer<NextIndexType>::get_read_block(
    const NextBlockIndex& block_index)
    throw(BaseException, CorruptedRecord)
  {
    return get_write_block(block_index);
  }

  template<typename NextIndexType>
  WriteBlock_var
  WriteCacheLayer<NextIndexType>::get_write_block(
    const NextIndexType& block_index)
    throw(BaseException, CorruptedRecord)
  {
    /* search for opened blocks */
    {
      typename WriteBlockTable::const_iterator exist_it =
        block_table_.find(block_index);
      if(exist_it != block_table_.end())
      {
        return exist_it->second;
      }
    }

    WriteBlock_var ret = this->next_layer()->get_write_block(block_index);

    block_table_.insert(typename WriteBlockTable::value_type(
      block_index, ret));

    return ret;
  }

  template<typename NextIndexType>
  unsigned long
  WriteCacheLayer<NextIndexType>::area_size() const throw()
  {
    return this->next_layer()->area_size();
  }

  template<typename NextIndexType>
  void
  WriteCacheLayer<NextIndexType>::allocate(
    typename BlockAllocator<NextIndexType>::AllocatedBlock& alloc_result)
    throw(BaseException)
  {
    allocator_->allocate(alloc_result);

    alloc_result.block = alloc_result.block;

    block_table_.insert(typename WriteBlockTable::value_type(
      alloc_result.block_index, alloc_result.block));
  }
} // namespace PlainStorage
