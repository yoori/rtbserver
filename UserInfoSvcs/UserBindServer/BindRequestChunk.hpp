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

#ifndef USERINFOSVCS_BINDREQUESTCHUNK_HPP
#define USERINFOSVCS_BINDREQUESTCHUNK_HPP

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

#include "BindRequestProcessor.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  class BindRequestChunk:
    public BindRequestProcessor,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    BindRequestChunk(
      Logging::Logger* logger,
      const char* file_root,
      const char* file_prefix,
      const Generics::Time& extend_time_period,
      unsigned long portions_number)
      throw(Exception);

    // return previous state
    void
    add_bind_request(
      const String::SubString& id,
      const BindRequest& bind_request,
      const Generics::Time& now)
      throw();

    BindRequest
    get_bind_request(
      const String::SubString& external_id,
      const Generics::Time& now)
      throw();

    void
    clear_expired(const Generics::Time& bound_expire_time)
      throw();

    virtual void
    dump() throw(Exception);

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;
    typedef Sync::Policy::PosixThread FlushSyncPolicy;
    typedef Sync::Policy::PosixThread ExtendSyncPolicy;

    class BindRequestHolder
    {
    public:
      BindRequestHolder();

      BindRequestHolder(const std::vector<std::string>& bind_user_ids);

      void
      save(std::ostream& out) const throw();

      void
      load(std::istream& input) throw(Exception);

      const std::vector<std::string>&
      bind_user_ids() const;

    private:
      std::vector<std::string> bind_user_ids_;
    };

    class HashHashAdapter
    {
    public:
      HashHashAdapter(size_t hash_val = 0) throw();

      template<typename HashAdapterType>
      HashHashAdapter(const HashAdapterType& init);

      bool
      operator==(const HashHashAdapter& right) const throw();

      size_t
      hash() const throw();

    protected:
      size_t hash_;
    };

    template<typename HashAdapterType, typename HolderType>
    struct HolderContainer: public ReferenceCounting::AtomicCopyImpl
    {
      typedef Generics::GnuHashTable<
        HashAdapterType, HolderType>
        HolderMap;

      typedef HashAdapterType KeyType;
      typedef HolderType MappedType;

      struct TimePeriodHolder: public ReferenceCounting::AtomicImpl
      {
        TimePeriodHolder(
          const Generics::Time& min_time_val,
          const Generics::Time& max_time_val)
          throw();

        const Generics::Time min_time;
        const Generics::Time max_time;

        mutable SyncPolicy::Mutex lock;
        HolderMap holders;

      protected:
        virtual ~TimePeriodHolder() throw()
        {}
      };

      typedef ReferenceCounting::SmartPtr<TimePeriodHolder>
        TimePeriodHolder_var;

      typedef std::vector<TimePeriodHolder_var>
        TimePeriodHolderArray;

      // sorted in max_time descending order
      TimePeriodHolderArray time_holders;

    protected:
      virtual ~HolderContainer() throw()
      {}
    };

    template<typename HolderContainerType>
    class HolderContainerGuard
    {
    public:
      typedef ReferenceCounting::SmartPtr<HolderContainerType>
        HolderContainer_var;

    public:
      mutable ExtendSyncPolicy::Mutex extend_lock;

    public:
      HolderContainerGuard() throw();

      HolderContainer_var
      holder_container() const throw();

      void
      swap_holder_container(HolderContainer_var& new_holder_container)
        throw();

    protected:
      mutable SyncPolicy::Mutex holder_container_lock_;
      HolderContainer_var holder_container_;
    };

    //
    typedef HolderContainer<HashHashAdapter, BindRequestHolder>
      BindRequestHolderContainer;

    typedef ReferenceCounting::SmartPtr<BindRequestHolderContainer>
      BindRequestHolderContainer_var;

    typedef AdServer::Commons::NoAllocLockMap<
      HashHashAdapter,
      Sync::Policy::PosixThread>
      UserLockMap;

    struct Portion: public ReferenceCounting::AtomicImpl
    {
    public:
      UserLockMap holders_lock;

      HolderContainerGuard<BindRequestHolderContainer> holder_container_guard;

    protected:
      virtual ~Portion() throw()
      {}
    };

    typedef ReferenceCounting::SmartPtr<Portion>
      Portion_var;

    typedef std::vector<Portion_var>
      PortionArray;

    typedef std::map<std::string, std::string> TempFilePathMap;

    typedef std::set<std::string> FileNameSet;

  protected:
    virtual
    ~BindRequestChunk() throw();

    HashHashAdapter
    get_external_id_hash_(
      unsigned long& portion,
      const String::SubString& external_id) const throw();

    unsigned long
    get_external_id_portion_(unsigned long full_hash) const
      throw();

    template<typename HolderContainerType>
    typename HolderContainerType::TimePeriodHolder_var
    get_holder_(
      HolderContainerGuard<HolderContainerType>& holder_container_guard,
      const HolderContainerType* holder_container,
      const Generics::Time& time,
      const Generics::Time& extend_time_period)
      throw();

    template<typename HolderContainerType>
    void
    get_holder_i_(
      typename HolderContainerType::TimePeriodHolder_var& res_holder,
      const HolderContainerType* holder_container,
      const Generics::Time& time)
      throw();

    template<typename HolderContainerType>
    typename HolderContainerType::TimePeriodHolderArray::iterator
    get_holder_i_(
      typename HolderContainerType::TimePeriodHolder_var& res_holder,
      HolderContainerType* holder_container,
      const Generics::Time& time)
      throw();

    template<typename HolderContainerType>
    void
    clear_expired_(
      HolderContainerGuard<HolderContainerType>& holder_container_guard,
      const Generics::Time& time)
      throw();

    template<typename HolderContainerType>
    void
    save_holders_i_(
      TempFilePathMap& result_files,
      HolderContainerGuard<HolderContainerType> Portion::* holder_container_guard_field,
      const FileNameSet& used_file_names)
      throw(Exception);

    template<typename HolderContainerType>
    void
    load_holders_(
      HolderContainerGuard<HolderContainerType> Portion::* holder_container_guard_field,
      const Generics::Time& extend_time_period)
      throw(Exception);

    void
    load_holders_() throw(Exception);

    void
    save_holders_() throw(Exception);

    void
    save_key_(std::ostream& out, const HashHashAdapter& key);

    void
    load_key_(
      HashHashAdapter& res,
      unsigned long& portion,
      std::istream& in) const
      throw();

    void
    generate_file_name_(
      std::string& persistent_file_name,
      std::string& tmp_file_name,
      const Generics::Time& max_time,
      const std::set<std::string>& used_file_names)
      const
      throw();

  private:
    const Logging::Logger_var logger_;
    const std::string file_root_;
    const std::string file_prefix_;
    const Generics::Time extend_time_period_;

    PortionArray portions_;

    // serialize saving to files
    mutable FlushSyncPolicy::Mutex flush_lock_;
  };

  typedef ReferenceCounting::SmartPtr<BindRequestChunk>
    BindRequestChunk_var;

} /* UserInfoSvcs */
} /* AdServer */

namespace AdServer
{
namespace UserInfoSvcs
{
  // HashHashAdapter
  inline
  BindRequestChunk::HashHashAdapter::HashHashAdapter(
    size_t hash_val) throw()
    : hash_(hash_val)
  {}

  template<typename HashAdapterType>
  BindRequestChunk::HashHashAdapter::HashHashAdapter(
    const HashAdapterType& init)
    : hash_(init.hash())
  {}

  inline
  bool
  BindRequestChunk::HashHashAdapter::operator==(
    const HashHashAdapter& right) const throw()
  {
    return hash_ == right.hash_;
  }

  inline
  size_t
  BindRequestChunk::HashHashAdapter::hash() const throw()
  {
    return hash_;
  }
}
}

#endif /*USERINFOSVCS_BINDREQUESTCHUNK_HPP*/
