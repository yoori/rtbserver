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
 * @file PlainStorage/MemLayer.hpp
 * MemLayer implementation of fixed size records layer (RAM based)
 */
#ifndef PLAINSTORAGE_MEMLAYER_HPP
#define PLAINSTORAGE_MEMLAYER_HPP

#include <functional>
#include <memory>

#include <Sync/SyncPolicy.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Generics/Time.hpp>

#include <Commons/LockMap.hpp>
#include "BaseLayer.hpp"

namespace PlainStorage
{
  typedef u_int64_t MemBlockIndex;

  /**
   * WriteMemLayer
   */
  class WriteMemLayer: public virtual ExWriteBaseLayer<MemBlockIndex>
  {
  public:
    DECLARE_EXCEPTION(Exception, BaseException);

  public:
    /**
     * Constructor
     * @param block_size The size for Data block
     */
    WriteMemLayer(unsigned long block_size) throw(Exception);

    /**
     * Empty virtual destructor
     */
    virtual ~WriteMemLayer() throw();

    virtual
    ReadBlock_var
    get_read_block(const MemBlockIndex& index) throw(BaseException);

    virtual
    WriteBlock_var
    get_write_block(const MemBlockIndex& index) throw(BaseException);

    virtual
    bool
    get_property(const char* name, PropertyValue& property) throw(BaseException);

    virtual
    void
    set_property(const char* name, const PropertyValue& property)
      throw(BaseException);

    virtual
    unsigned long
    area_size() const throw();

    unsigned long block_data_size() const throw ();

    virtual MemBlockIndex max_block_index() const throw();

  protected:
    class WriteBlockImpl;
    friend class WriteBlockImpl;

  private:
    typedef std::map<MemBlockIndex, WriteBlock_var> BlockMap;

    typedef std::map<std::string, PropertyValue> PropertyMap;

    typedef Sync::Policy::PosixThread SyncPolicy;

  private:
    /// Requested Data block size
    const unsigned int block_size_;

    mutable SyncPolicy::Mutex blocks_lock_;
    BlockMap blocks_;

    mutable SyncPolicy::Mutex properties_lock_;
    PropertyMap properties_;
  };

  typedef ReferenceCounting::SmartPtr<WriteMemLayer> WriteMemLayer_var;
}

#endif // PLAINSTORAGE_MEMLAYER_HPP
