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

#ifndef PROFILING_COMMONS_CHUNKED_EXPIRE_PROFILE_MAP_HPP
#define PROFILING_COMMONS_CHUNKED_EXPIRE_PROFILE_MAP_HPP

#include <vector>
#include <set>
#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>

/**
 * ChunkedUserMap:
 *   ChunkedProfileMap -> TransactionProfileMap ->
 *   AdaptProfileMap -> ExpireProfileMap
 */
namespace AdServer
{
namespace ProfilingCommons
{
  typedef std::set<unsigned long> ChunkIdSet;

  template<typename KeyType,
    typename ProfileMapType,
    typename KeyHashType>
  class ChunkedProfileMap:
    public virtual ProfileMap<KeyType>,
    public virtual ReferenceCounting::AtomicImpl
  {
    /* requirements:
     * KeyType must be convertible to ProfileMapType::KeyTypeT */
  public:
    typedef ProfileMapType BaseProfileMap;
    typedef ReferenceCounting::SmartPtr<BaseProfileMap> BaseProfileMap_var;
    typedef typename ProfileMapType::Transaction_var Transaction_var;

    typedef std::map<unsigned long, ReferenceCounting::SmartPtr<ProfileMapType> >
      ChunkIdToProfileMap;

    typedef std::vector<BaseProfileMap_var> ChunkList;

    DECLARE_EXCEPTION(Exception, typename ProfileMap<KeyType>::Exception);
    DECLARE_EXCEPTION(ChunkNotFound, Exception);

    ChunkedProfileMap(
      unsigned long common_chunks_number,
      const ChunkIdToProfileMap& chunks,
      const KeyHashType& key_hash = KeyHashType())
      throw(Exception, typename BaseProfileMap::Exception);

    bool
    dispose_profile(const KeyType& key) const
      throw();

    virtual void
    wait_preconditions(
      const KeyType& key,
      OperationPriority priority) const
      throw(ChunkNotFound, Exception);

    bool
    check_profile(const KeyType& key) const
      throw(ChunkNotFound, Exception);

    Generics::ConstSmartMemBuf_var
    get_profile(
      const KeyType& key,
      Generics::Time* last_access_time = 0)
      throw(ChunkNotFound, Exception);

    void
    save_profile(
      const KeyType& key,
      const Generics::ConstSmartMemBuf* mem_buf,
      const Generics::Time& now = Generics::Time::get_time_of_day(),
      OperationPriority priority = OP_RUNTIME)
      throw(ChunkNotFound, Exception);

    bool
    remove_profile(
      const KeyType& key,
      OperationPriority priority = OP_RUNTIME)
      throw(ChunkNotFound, Exception);

    Transaction_var
    get_transaction(
      const KeyType& key,
      bool check_max_waiters = true,
      OperationPriority op_priority = ProfilingCommons::OP_RUNTIME)
      throw(ChunkNotFound,
        typename BaseProfileMap::MaxWaitersReached,
        typename BaseProfileMap::Exception);

    virtual void
    copy_keys(typename ProfileMap<KeyType>::KeyList& keys) throw(Exception);

    void clear_expired(const Generics::Time& expire_time)
      throw(Exception);

    unsigned long size() const throw();

    unsigned long area_size() const throw();

    const ChunkIdSet& chunk_ids() const throw();

    BaseProfileMap_var get_chunk(unsigned long chunk_id) const
      throw(ChunkNotFound);

    const ChunkList& chunks() const throw();

  protected:
    virtual ~ChunkedProfileMap() throw() {}
      
    BaseProfileMap_var get_chunk_(unsigned long chunk_id) const
      throw();

  private:
    const KeyHashType key_hash_;
    unsigned long common_chunks_number_;
    ChunkIdSet chunk_ids_;
    ChunkList chunks_;
  };
}
}

#include "ChunkedExpireProfileMap.tpp"

#endif /*PROFILING_COMMONS_CHUNKED_EXPIRE_PROFILE_MAP_HPP*/
