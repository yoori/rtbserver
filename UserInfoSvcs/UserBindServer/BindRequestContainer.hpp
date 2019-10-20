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

#ifndef USERINFOSVCS_BINDREQUESTCONTAINER_HPP
#define USERINFOSVCS_BINDREQUESTCONTAINER_HPP

#include <string>

#include <eh/Exception.hpp>

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>

#include <Logger/Logger.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Generics/Time.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Generics/HashTableAdapters.hpp>
#include <Generics/Hash.hpp>

#include <Commons/UserInfoManip.hpp>
#include <Commons/LockMap.hpp>
#include <Commons/Containers.hpp>

#include "BindRequestChunk.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  class BindRequestContainer:
    public BindRequestProcessor,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    typedef std::map<unsigned long, std::string> ChunkPathMap;

  public:
    BindRequestContainer(
      Logging::Logger* logger,
      unsigned long common_chunks_number,
      const ChunkPathMap& chunk_folders,
      const char* file_prefix,
      const Generics::Time& extend_time_period,
      unsigned long portions_number)
      throw(Exception);

    // BindRequestProcessor impl
    virtual void
    add_bind_request(
      const String::SubString& id,
      const BindRequest& bind_request,
      const Generics::Time& now)
      throw(ChunkNotFound, Exception);

    virtual BindRequest
    get_bind_request(
      const String::SubString& external_id,
      const Generics::Time& now)
      throw(ChunkNotFound, Exception);

    virtual void
    clear_expired(const Generics::Time& expire_time)
      throw(Exception);

    virtual void
    dump() throw(Exception);

  protected:
    typedef std::vector<BindRequestChunk_var> ChunkArray;

  protected:
    virtual
    ~BindRequestContainer() throw();

    BindRequestChunk_var
    get_chunk_(const String::SubString& external_id)
      const throw(ChunkNotFound);

  private:
    const Logging::Logger_var logger_;
    const unsigned long common_chunks_number_;
    ChunkArray chunks_;
  };

  typedef ReferenceCounting::SmartPtr<BindRequestContainer>
    BindRequestContainer_var;

} /* UserInfoSvcs */
} /* AdServer */

#endif /*USERINFOSVCS_BINDREQUESTCONTAINER_HPP*/
