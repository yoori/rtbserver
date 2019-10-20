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

#ifndef USERINFOSVCS_USERBINDCHUNK_HPP
#define USERINFOSVCS_USERBINDCHUNK_HPP

#include <string>
#include <set>

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
#include <Commons/tsl/sparse_map.h>
#include <Commons/tsl/sparse_set.h>

#include "FetchableHashTable.hpp"
#include "ExternalIdHashAdapter.hpp"
#include "UserBindProcessor.hpp"

namespace AdServer
{
namespace UserInfoSvcs
{
  class UserBindChunk:
    public UserBindProcessor,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    UserBindChunk(
      Logging::Logger* logger,
      const char* file_root,
      const char* file_prefix,
      const char* bound_file_prefix,
      const Generics::Time& extend_time_period,
      const Generics::Time& bound_extend_time_period,
      const Generics::Time& min_bind_age,
      bool bind_at_min_age,
      unsigned long max_bad_event,
      unsigned long portions_number,
      bool load_slave,
      unsigned long partition_index, // instance partition number (first or second part of cluster)
      unsigned long partitions_number,
      unsigned long local_chunks = 1)
      throw(Exception);

    // return previous state
    UserInfo
    add_user_id(
      const String::SubString& external_id,
      const Commons::UserId& user_id,
      const Generics::Time& now,
      bool resave_if_exists,
      bool ignore_bad_event)
      throw();

    UserInfo
    get_user_id(
      const String::SubString& external_id,
      const Commons::UserId& current_user_id,
      const Generics::Time& now,
      bool silent,
      const Generics::Time& create_time,
      bool for_set_cookie)
      throw();

    void
    add_bind_request(
      const Commons::RequestId& request_id,
      const String::SubString& bind_str,
      const Generics::Time& now)
      throw();

    std::string
    get_bind_request(
      const Commons::RequestId& request_id)
      throw();

    void
    clear_expired(
      const Generics::Time& unbound_expire_time,
      const Generics::Time& bound_expire_time)
      throw();

    virtual void
    dump() throw(Exception);

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;
    typedef Sync::Policy::PosixThread FlushSyncPolicy;
    typedef Sync::Policy::PosixThread ExtendSyncPolicy;

    class UserInfoHolder
    {
    public:
      UserInfoHolder();

      bool
      need_save() const
      {
        return true;
      }

      void
      save(std::ostream& out, const Generics::Time& base_time) const throw();

      void
      load(std::istream& input, const Generics::Time& base_time) throw(Exception);

      Generics::Time
      get_time(const Generics::Time& base_time) const;

      void
      set_time(const Generics::Time& time, const Generics::Time& base_time);

      void
      adjust_time(const Generics::Time& old_base_time, const Generics::Time& new_base_time);

      bool
      is_init() const;

    private:
      typedef int32_t TimeOffset;

    private:
      static const TimeOffset TIME_OFFSET_NOT_INIT_;
      static const Generics::Time TIME_OFFSET_MIN_;
      static const Generics::Time TIME_OFFSET_MAX_;

      TimeOffset first_seen_time_offset_;
    };

    struct BoundUserInfoHolder
    {
      enum BoundFlags
      {
        BF_SETCOOKIE = 1
      };

      BoundUserInfoHolder() throw();

      bool
      need_save() const /*throw()*/;

      void
      save(std::ostream& out, const Generics::Time& base_time) const
        throw ();

      void
      load(std::istream& input, const Generics::Time& base_time)
        throw(Exception);

      Commons::UserId user_id;
      unsigned char flags;
      uint8_t bad_event_count;
      uint16_t last_bad_event_day;
    }
#   ifdef __GNUC__
    __attribute__ ((packed))
#   endif
    ;

    // String hash holder with predefined hash value
    class StringDefHashAdapter
    {
    public:
      StringDefHashAdapter() throw();

      StringDefHashAdapter(const String::SubString& text, size_t hash)
        throw();

      StringDefHashAdapter(StringDefHashAdapter&& init) throw();

      StringDefHashAdapter(const StringDefHashAdapter& init) throw();

      StringDefHashAdapter&
      operator=(const StringDefHashAdapter& init) throw();

      StringDefHashAdapter&
      operator=(StringDefHashAdapter&& init) throw();

      ~StringDefHashAdapter() throw();

      bool
      operator==(const StringDefHashAdapter& right) const throw();

      size_t
      hash() const throw();

      std::string
      text() const throw();

    protected:
      void* data_;
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

    /*
    struct DefaultIteratorValueAdapter
    {
      template<typename IteratorType>
      typename IteratorType::value_type::second_type&
      operator()(IteratorType& it)
      {
        return it->second;
      }
    };

    template<typename ValueType>
    struct SparseMapIteratorValueAdapter
    {
      template<typename IteratorType>
      ValueType&
      operator()(IteratorType& it)
      {
        return it.value();
      }
    };
    */

    template<
      typename HashAdapterType,
      typename UserInfoHolderType,
      template<typename, typename> class HashTableType = USFetchableHashTable
      >
    struct HolderContainer: public ReferenceCounting::AtomicCopyImpl
    {
      typedef HashTableType<HashAdapterType, UserInfoHolderType>
        UserIdMap;

      typedef HashAdapterType KeyType;
      typedef UserInfoHolderType MappedType;
      //typedef IteratorValueAdapterType IteratorValueAdapter;

      struct TimePeriodHolder: public ReferenceCounting::AtomicImpl
      {
        typedef HolderContainer<
          HashAdapterType, UserInfoHolderType, HashTableType>::UserIdMap
          UserIdMap;

        TimePeriodHolder(
          const Generics::Time& min_time_val,
          const Generics::Time& max_time_val)
          throw();

        const Generics::Time min_time;
        const Generics::Time max_time;

        //mutable SyncPolicy::Mutex lock;
        UserIdMap users;

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
    typedef HolderContainer<HashHashAdapter, UserInfoHolder>
      SeenUserHolderContainer;

    typedef ReferenceCounting::SmartPtr<
      SeenUserHolderContainer>
      SeenUserHolderContainer_var;

    struct HashByHashHolder
    {
      template<typename HashHolderType>
      size_t
      operator()(const HashHolderType& val) const
      {
        return val.hash();
      }
    };

    typedef HolderContainer<
      ExternalIdHashAdapter, // StringDefHashAdapter,
      BoundUserInfoHolder,
      SparseFetchableHashTable>
      BoundUserHolderContainer;

    typedef ReferenceCounting::SmartPtr<
      BoundUserHolderContainer>
      BoundUserHolderContainer_var;

    typedef AdServer::Commons::LockMap<
      StringDefHashAdapter,
      Sync::Policy::PosixThread>
      UserLockMap;

    struct Portion: public ReferenceCounting::AtomicImpl
    {
    public:
      struct BoundUserHolderContainerGuard:
        public HolderContainerGuard<BoundUserHolderContainer>,
        public ReferenceCounting::AtomicImpl
      {};

      typedef ReferenceCounting::SmartPtr<BoundUserHolderContainerGuard>
        BoundUserHolderContainerGuard_var;

      typedef Generics::GnuHashTable<
        Generics::StringHashAdapter,
        BoundUserHolderContainerGuard_var>
        BoundUserHolderSourceMap;

      typedef Sync::Policy::PosixThreadRW SourceSyncPolicy;

    public:
      Portion(unsigned long up_user_divisions)
        : users_lock(std::min(1000ul, std::max(5ul, 1024 * 128 / up_user_divisions)))
      {}

    public:
      UserLockMap users_lock;

      HolderContainerGuard<SeenUserHolderContainer> holder_container_guard;

      SourceSyncPolicy::Mutex source_lock;
      BoundUserHolderSourceMap source_to_bound_user_holder_container_guards;

      //HolderContainerGuard<BoundUserHolderContainer> bound_user_holder_container_guard;

    protected:
      virtual ~Portion() throw()
      {}
    };

    struct ResultHolder
    {
      ResultHolder(bool invalid_operation_, bool user_found_)
       : invalid_operation(invalid_operation_),
         user_found(user_found_)
      {}

      bool invalid_operation;
      bool user_found;
    };

    typedef ReferenceCounting::SmartPtr<Portion>
      Portion_var;

    typedef std::vector<Portion_var>
      PortionArray;

    typedef std::map<std::string, std::string> TempFilePathMap;

    typedef std::set<std::string> FileNameSet;

    struct PortionLoad;

    template<typename TimePeriodHolderType>
    struct SaveTimePeriodHolder
    {
      SaveTimePeriodHolder(
        const String::SubString& key_prefix_val,
        TimePeriodHolderType* time_holder_val)
        : key_prefix(key_prefix_val.str()),
          time_holder(ReferenceCounting::add_ref(time_holder_val))
      {}

      std::string key_prefix;
      ReferenceCounting::SmartPtr<TimePeriodHolderType> time_holder;
    };

    typedef std::list<SaveTimePeriodHolder<SeenUserHolderContainer::TimePeriodHolder> >
      SeenSaveTimePeriodHolderList;

    typedef std::map<Generics::Time, SeenSaveTimePeriodHolderList>
      SeenSaveTimePeriodHolderMap;

    typedef std::list<SaveTimePeriodHolder<BoundUserHolderContainer::TimePeriodHolder> >
      BoundSaveTimePeriodHolderList;

    typedef std::map<Generics::Time, BoundSaveTimePeriodHolderList>
      BoundSaveTimePeriodHolderMap;

  protected:
    virtual ~UserBindChunk() throw();

    StringDefHashAdapter
    get_external_id_hash_(
      unsigned long& portion,
      const String::SubString& external_id) const throw();

    ExternalIdHashAdapter // StringDefHashAdapter
    get_bound_external_id_hash_(
      const String::SubString& external_id) const throw();

    unsigned long
    get_external_id_portion_(unsigned long full_hash) const
      throw();

    template<typename HolderContainerType>
    void
    erase_user_id_(
      HolderContainerGuard<HolderContainerType>& holder_container_guard,
      const StringDefHashAdapter& external_id_hash)
      throw();

    template<typename HolderContainerType, typename KeyType>
    bool
    get_user_id_(
      typename HolderContainerType::MappedType& user_info,
      bool& insert_into_bound,
      bool& inserted,
      ResultHolder& result_holder,
      Generics::Time& base_time,
      HolderContainerGuard<HolderContainerType>& holder_container_guard,
      const KeyType& external_id_hash,
      const Commons::UserId& current_user_id,
      const Generics::Time& now,
      const Generics::Time& create_time,
      bool insert_if_not_found,
      bool silent,
      bool for_set_cookie,
      const Generics::Time& extend_time_period)
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

    template<typename TimePeriodHolderPtrType>
    void
    save_time_holders_(
      TempFilePathMap& result_files,
      const std::map<Generics::Time, std::list<SaveTimePeriodHolder<TimePeriodHolderPtrType> > >&
        time_period_holders_by_time,
      const char* file_prefix,
      const FileNameSet& used_file_names)
      throw(Exception);

    void
    load_seen_users_(
      const char* file_root,
      const char* file_prefix,
      const Generics::Time& extend_time_period)
      throw(Exception);

    void
    load_bound_users_(
      const char* file_root,
      const char* file_prefix,
      const Generics::Time& extend_time_period)
      throw(Exception);

    bool
    modify_at_get_(
      UserInfoHolder& user_info,
      const Commons::UserId& current_user_id,
      const Generics::Time& now,
      const Generics::Time& base_time,
      const Generics::Time* const old_base_time,
      const Generics::Time* const create_time,
      bool for_set_cookie,
      bool silent) const
      throw();

    bool
    modify_at_get_(
      BoundUserInfoHolder& user_info,
      const Commons::UserId& current_user_id,
      const Generics::Time& now,
      const Generics::Time& base_time,
      const Generics::Time* const old_base_time,
      const Generics::Time* const create_time,
      bool for_set_cookie,
      bool silent) const
      throw();

    static bool
    prepare_result_at_get_(
      UserInfoHolder& result,
      ResultHolder& result_holder,
      bool for_set_cookie)
      throw();

    bool
    prepare_result_at_get_(
      BoundUserInfoHolder& result,
      ResultHolder& result_holder,
      bool for_set_cookie)
      throw();

    UserInfo
    adapt_user_info_(
      UserInfoHolder& user_info,
      bool inserted,
      const ResultHolder& result_holder,
      const Generics::Time& now,
      const Generics::Time& base_time) const
      throw();

    UserInfo
    adapt_user_info_(
      BoundUserInfoHolder& user_info,
      bool user_id_generated,
      const ResultHolder& result_holder) const
      throw();

    void
    save_user_id_(
      BoundUserInfoHolder& user_info,
      UserInfo& res_user_info,
      const Commons::UserId& user_id,
      bool ignore_bad_event,
      const Generics::Time& now) const
      throw();

    void
    rotate_bad_event_count_(
      BoundUserInfoHolder& user_info,
      const Generics::Time& now) const
      throw();

    void
    load_users_() throw(Exception);

    void
    save_users_() throw(Exception);

    void
    remove_chunk_(const char* path)
      throw(Exception);

    void
    rename_to_prefixed_(const char* path)
      throw(Exception);

    void
    save_key_(std::ostream& out, const ExternalIdHashAdapter& key);

    bool
    load_key_(
      StringDefHashAdapter& res,
      unsigned long& portion,
      std::istream& in) const
      throw();

    void
    save_key_(std::ostream& out, const HashHashAdapter& key);

    bool
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
      const char* file_prefix,
      const std::set<std::string>& used_file_names)
      const
      throw();

    static void
    div_external_id_(
      String::SubString& external_id_prefix,
      String::SubString& external_id_suffix,
      const String::SubString& external_id)
      throw();

    Portion::BoundUserHolderContainerGuard_var
    get_bound_user_holder_container_guard_(
      Portion* portion,
      const String::SubString& external_id_prefix)
      throw();

    bool
    need_to_load_(const String::SubString&) const throw();

  private:
    const Logging::Logger_var logger_;
    const std::string file_root_;
    const std::string file_prefix_;
    const std::string bound_file_prefix_;
    const Generics::Time extend_time_period_;
    const Generics::Time bound_extend_time_period_;
    const Generics::Time min_age_;
    const bool bind_at_min_age_;
    const unsigned long max_bad_event_;
    const bool load_slave_;
    const unsigned long partition_index_;
    const unsigned long partitions_number_;

    PortionArray portions_;

    // serialize saving to files
    mutable FlushSyncPolicy::Mutex flush_lock_;
  };

  typedef ReferenceCounting::SmartPtr<UserBindChunk>
    UserBindChunk_var;

} /* UserInfoSvcs */
} /* AdServer */

namespace AdServer
{
namespace UserInfoSvcs
{
  // StringDefHashAdapter
  inline
  UserBindChunk::StringDefHashAdapter::
  StringDefHashAdapter() throw()
    : data_(0)
  {}

  inline
  UserBindChunk::StringDefHashAdapter::
  StringDefHashAdapter(const String::SubString& text, size_t hash)
    throw()
  {
    data_ = ::malloc(4 + text.size() + 1);
    *static_cast<uint32_t*>(data_) = hash;
    ::memcpy(static_cast<unsigned char*>(data_) + 4, text.data(), text.size());
    static_cast<char*>(data_)[4 + text.size()] = 0;
  }

  inline
  UserBindChunk::StringDefHashAdapter::
  StringDefHashAdapter(StringDefHashAdapter&& init)
    throw()
  {
    data_ = init.data_;
    init.data_ = 0;
  }

  inline
  UserBindChunk::StringDefHashAdapter::
  StringDefHashAdapter(const StringDefHashAdapter& init)
    throw()
  {
    if(init.data_)
    {
      int ssize = ::strlen(static_cast<char*>(init.data_) + 4);
      data_ = ::malloc(4 + ssize + 1);
      ::memcpy(data_, init.data_, 4 + ssize + 1);
    }
    else
    {
      data_ = 0;
    }
  }

  inline
  UserBindChunk::StringDefHashAdapter&
  UserBindChunk::StringDefHashAdapter::
  operator=(const StringDefHashAdapter& init)
    throw()
  {
    if(data_)
    {
      ::free(data_);
    }

    if(init.data_)
    {
      int ssize = ::strlen(static_cast<char*>(init.data_) + 4);
      data_ = ::malloc(4 + ssize + 1);
      ::memcpy(data_, init.data_, 4 + ssize + 1);
    }
    else
    {
      data_ = 0;
    }

    return *this;
  }

  inline
  UserBindChunk::StringDefHashAdapter&
  UserBindChunk::StringDefHashAdapter::
  operator=(StringDefHashAdapter&& init)
    throw()
  {
    if(data_)
    {
      ::free(data_);
    }

    data_ = init.data_;
    init.data_ = 0;

    return *this;
  }

  inline
  UserBindChunk::StringDefHashAdapter::
  ~StringDefHashAdapter() throw()
  {
    if(data_)
    {
      ::free(data_);
    }
  }

  inline
  bool
  UserBindChunk::StringDefHashAdapter::
  operator==(const StringDefHashAdapter& right) const
    throw()
  {
    return ::strcmp(static_cast<char*>(data_) + 4, static_cast<char*>(right.data_) + 4) == 0;
  }

  inline
  size_t
  UserBindChunk::StringDefHashAdapter::
  hash() const
    throw()
  {
    return data_ ? *static_cast<uint32_t*>(data_) : 0;
  }

  inline
  std::string
  UserBindChunk::StringDefHashAdapter::
  text() const
    throw()
  {
    return data_ ? std::string(static_cast<char*>(data_) + 4) : std::string();
  }

  // HashHashAdapter
  inline
  UserBindChunk::HashHashAdapter::HashHashAdapter(
    size_t hash_val) throw()
    : hash_(hash_val)
  {}

  template<typename HashAdapterType>
  UserBindChunk::HashHashAdapter::HashHashAdapter(
    const HashAdapterType& init)
    : hash_(init.hash())
  {}

  inline
  bool
  UserBindChunk::HashHashAdapter::operator==(
    const HashHashAdapter& right) const throw()
  {
    return hash_ == right.hash_;
  }

  inline
  size_t
  UserBindChunk::HashHashAdapter::hash() const throw()
  {
    return hash_;
  }
}
}

#endif /*USERINFOSVCS_USERBINDCHUNK_HPP*/
