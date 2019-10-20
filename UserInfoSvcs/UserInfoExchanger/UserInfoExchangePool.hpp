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

#ifndef _ADSERVER_USER_INFO_SVCS_USER_INFO_EXCHANGE_POOL_HPP_
#define _ADSERVER_USER_INFO_SVCS_USER_INFO_EXCHANGE_POOL_HPP_

#include <list>

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/List.hpp>
#include <Generics/ArrayAutoPtr.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include <Commons/UserInfoManip.hpp>
#include <ProfilingCommons/ProfileMap/ProfileMapFactory.hpp>

#include <UserInfoSvcs/UserInfoCommons/Allocator.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    /** UserInfoExchangePool */
    class UserInfoExchangePool:
      public virtual ReferenceCounting::AtomicImpl,
      public Generics::CompositeActiveObject
    {
    public:
      struct ReceiveCriteria
      {
        typedef std::list<unsigned long> ChunkIdList;
        ChunkIdList chunk_ids;
        unsigned long common_chunks_number;
        unsigned long max_response_plain_size;
      };

      struct UserProfile
      {
        UserProfile() throw();
        UserProfile(UserProfile&& init) throw();

        std::string user_id;
        std::string provider_id;

        Generics::ArrayChar plain_user_info;
        unsigned long plain_size;

        Generics::ArrayChar plain_history_user_info;
        unsigned long plain_history_size;
      };

      typedef std::list<std::string> UserIdList;
      typedef ReferenceCounting::List<UserProfile> UserProfileList;

    protected:
      /**
       * ProviderStorage - class
       */
      class ProviderStorage:
        public virtual ReferenceCounting::AtomicImpl,
        public virtual Generics::CompositeActiveObject
      {
      public:
        typedef AdServer::ProfilingCommons::ProfileMap<Generics::StringHashAdapter>
          UserMap;

        typedef ReferenceCounting::SmartPtr<UserMap> UserMap_var;

        struct UserInfoRef:
          public virtual ReferenceCounting::AtomicImpl
        {
        public:
          UserInfoRef(
            ProviderStorage* storage,
            const char* user_id_val) throw();

          virtual ~UserInfoRef() throw();

          void read(UserProfile& user_profile)
            throw(eh::Exception);

          const char* user_id() const throw();

        protected:
          ReferenceCounting::SmartPtr<ProviderStorage> storage_;
          std::string user_id_;
        };

        typedef ReferenceCounting::SmartPtr<UserInfoRef> UserInfoRef_var;

      public:
        ProviderStorage(
          Generics::ActiveObjectCallback* callback,
          const char* base_path,
          const char* base_prefix,
          const char* history_path,
          const char* history_prefix,
          const Generics::Time extend_time)
          throw(eh::Exception);

        virtual ~ProviderStorage() throw();

        void
        read(const char* user_id, UserProfile& user_profile)
          throw(eh::Exception);

        UserInfoRef*
        insert_i(const UserProfile& user_profile)
          throw(eh::Exception);

        void
        erase_old_profiles(const Generics::Time expire_time)
          throw(eh::Exception);

      protected:
        void erase_(const char* user_id) throw();

      protected:
        typedef Sync::PosixRWLock Mutex_;
        typedef Sync::PosixRGuard ReadGuard_;
        typedef Sync::PosixWGuard WriteGuard_;
        typedef std::map<std::string, UserInfoRef*> UserRefMap;

        mutable Mutex_ lock_;

        UserRefMap user_refs_;

        UserMap_var user_profiles_;
        UserMap_var user_history_profiles_;
      };

      typedef ReferenceCounting::SmartPtr<ProviderStorage> ProviderStorage_var;

      /**
       * Customer
       */
      class Customer: public virtual ReferenceCounting::AtomicImpl
      {
      protected:
        typedef
          std::list<ProviderStorage::UserInfoRef_var>
          UserInfoRefList;

        struct UserInfoBlock
        {
          std::string provider_id;
          UserInfoRefList users;
        };

        typedef std::list<UserInfoBlock> UserInfoBlockList;

      public:
        Customer(const char* customer_id) throw();
        virtual ~Customer() throw();

        void receive_users(
          UserProfileList& user_profiles,
          const ReceiveCriteria& receive_criteria) throw(eh::Exception);

        void notify_users_receiving(
          const char* provider_id,
          const UserInfoRefList& users) throw(eh::Exception);

        const char* get_name() throw();

      protected:
        typedef Sync::PosixRWLock Mutex_;
        typedef Sync::PosixRGuard ReadGuard_;
        typedef Sync::PosixWGuard WriteGuard_;

        mutable Mutex_ lock_;

        std::string customer_id_;
        UserInfoBlockList received_user_blocks_;
      };

      typedef ReferenceCounting::SmartPtr<Customer> Customer_var;

      /**
       * Provider
       */
      class Provider:
        public virtual ReferenceCounting::AtomicImpl,
        public Generics::CompositeActiveObject
      {
      public:
        typedef std::set<Customer_var> CustomerSet;

        struct UserInfoRequest
        {
          CustomerSet customers;
        };

        typedef std::map<std::string, UserInfoRequest> UserInfoRequestMap;

      public:
        Provider(
          Generics::ActiveObjectCallback* callback,
          const char* plain_storage_path,
          const char* base_prefix,
          const char* plain_history_storage_path,
          const char* history_prefix,
          const Generics::Time extend_time) throw(eh::Exception);

        virtual ~Provider() throw();

        void
        register_users_request(Customer* customer, const UserIdList& users)
          throw(eh::Exception);

        void
        get_users_requests(UserIdList& users)
          throw(eh::Exception);

        void
        fill_users(const UserProfileList& user_profiles)
          throw(eh::Exception);

        void
        erase_old_profiles(const Generics::Time expire_time)
          throw(eh::Exception);

      protected:
        typedef Sync::PosixRWLock Mutex_;
        typedef Sync::PosixRGuard ReadGuard_;
        typedef Sync::PosixWGuard WriteGuard_;

        mutable Mutex_ lock_;

        std::string provider_id_;

        UserInfoRequestMap requested_users_;
        ProviderStorage_var storage_;
      };

      typedef
        ReferenceCounting::SmartPtr<Provider>
        Provider_var;

    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      UserInfoExchangePool(
        Generics::ActiveObjectCallback* callback,
        const char* providers_dir,
        const Generics::Time extend_time)
        throw(Exception);

      virtual ~UserInfoExchangePool() throw();

      void erase_old_profiles(
        const Generics::Time expire_time)
        throw(eh::Exception);

      void
      register_users_request(
        const char* customer_id,
        const char* provider_id,
        const UserIdList& users) throw(Exception);

      void
      receive_users(
        const char* customer_id,
        UserProfileList& users,
        const ReceiveCriteria& receive_criteria)
        throw(Exception);

      void
      get_users_request(
        const char* provider_id,
        UserIdList& users) throw(Exception);

      void
      add_users(
        const char* provider_id,
        const UserProfileList& users) throw(Exception);

    protected:
      Provider* get_provider()
        throw(Exception);

      Customer* get_customer(const char* customer_id) throw();

    protected:
      typedef Sync::PosixRWLock Mutex_;
      typedef Sync::PosixRGuard ReadGuard_;
      typedef Sync::PosixWGuard WriteGuard_;

      mutable Mutex_ lock_;

      typedef std::map<std::string, Customer_var> CustomerMap;

      Provider_var providers_;

      CustomerMap customers_;

      std::string providers_dir_;
    };

    typedef ReferenceCounting::SmartPtr<UserInfoExchangePool> UserInfoExchangePool_var;

  } /* UserInfoSvcs */
} /* AdServer */

namespace AdServer
{
  namespace UserInfoSvcs
  {
    /** UserInfoExchangePool::UserProfile */
    inline
    UserInfoExchangePool::
    UserProfile::UserProfile() throw()
        : plain_size(0)
    {}

    inline
    UserInfoExchangePool::
    UserProfile::UserProfile(
      UserInfoExchangePool::UserProfile&& init) throw()
        : user_id(init.user_id),
          provider_id(init.provider_id),
          plain_user_info(std::move(init.plain_user_info)),
          plain_size(init.plain_size),
          plain_history_user_info(std::move(init.plain_history_user_info)),
          plain_history_size(init.plain_history_size)
    {}

    /** UserInfoExchangePool::PlainStorage::UserInfoRef */
    inline
    UserInfoExchangePool::
    ProviderStorage::UserInfoRef::UserInfoRef(
      ProviderStorage* storage,
      const char* user_id) throw()
      : storage_(ReferenceCounting::add_ref(storage)),
        user_id_(user_id)
    {
    }

    inline
    UserInfoExchangePool::
    ProviderStorage::UserInfoRef::~UserInfoRef() throw()
    {
      storage_->erase_(user_id_.c_str());
    }

    inline
    const char*
    UserInfoExchangePool::
    ProviderStorage::UserInfoRef::user_id() const throw()
    {
      return user_id_.c_str();
    }

    /** UserInfoExchangePool::Customer */
    inline
    const char*
    UserInfoExchangePool::Customer::get_name() throw()
    {
      return customer_id_.c_str();
    }

  }
}

#endif /*_ADSERVER_USER_INFO_SVCS_USER_INFO_EXCHANGE_POOL_HPP_*/
