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

#ifndef LOGCOMMONS_LOGHOLDER_HPP
#define LOGCOMMONS_LOGHOLDER_HPP

#include <string>
#include <list>
#include <eh/Exception.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Stream/MemoryStream.hpp>
#include <Generics/Time.hpp>
#include <Generics/LastPtr.hpp>
#include <Sync/SyncPolicy.hpp>
#include <LogCommons/GenericLogIoImpl.hpp>
#include <LogCommons/GenericLogCsvSaverImpl.hpp>

namespace AdServer
{
  namespace LogProcessing
  {
    class LogHolder: public virtual ReferenceCounting::Interface
    {
    public:
      virtual Generics::Time flush_if_required(
        const Generics::Time& now) throw(eh::Exception) = 0;

    protected:
      virtual
      ~LogHolder() throw () = default;
    };

    typedef ReferenceCounting::SmartPtr<LogHolder> LogHolder_var;

    struct LogFlushTraits
    {
      LogFlushTraits() {}

      LogFlushTraits(
        const Generics::Time& period_val,
        const char* out_dir_val)
        : period(period_val),
          out_dir(out_dir_val)
      {}

      Generics::Time period;
      std::string out_dir;
    };

    template<typename LogTraitsType>
    struct DefaultSavePolicy
    {
      typedef
        AdServer::LogProcessing::GenericLogIoHelperImpl<LogTraitsType>
        IoHelperT;

      void save(
        typename LogTraitsType::CollectorType& collector,
        const char* out_dir)
        throw(eh::Exception)
      {
        IoHelperT(collector).save(out_dir);
      }
    };

    template<typename LogTraitsType>
    class DistributionSavePolicy
    {
    public:
      DistributionSavePolicy(unsigned long distrib_count = 24)
        : distrib_count_(distrib_count)
      {}

      void save(
        typename LogTraitsType::CollectorType& collector,
        const char* out_dir)
        throw(eh::Exception)
      {
        LogIoProxy<LogTraitsType>::save(collector, out_dir, distrib_count_);
      }

    private:
      unsigned long distrib_count_;
    };

    template<typename LogTraitsType>
    struct SimpleCsvSavePolicy
    {
      typedef
        AdServer::LogProcessing::SimpleLogCsvSaverImpl<LogTraitsType>
        CsvSaverT;

      void save(
        typename LogTraitsType::CollectorType& collector,
        const char* out_dir)
        throw(eh::Exception)
      {
        SimpleLogSaver_var(new CsvSaverT(collector, out_dir))->save();
      }
    };

    template<
      typename LogTraitsType,
      typename SavePolicy = DefaultSavePolicy<LogTraitsType> >
    class LogHolderImpl:
      public virtual LogHolder,
      public virtual ReferenceCounting::AtomicImpl
    {
    private:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    public:
      typedef typename LogTraitsType::CollectorType CollectorT;
      typedef Sync::Policy::PosixThread SyncPolicy;

    public:
      LogHolderImpl(
        const LogFlushTraits& flush_traits,
        const SavePolicy& save_policy = SavePolicy())
        throw(eh::Exception);

      Generics::Time flush_if_required(
        const Generics::Time& now) throw(eh::Exception);

    protected:
      virtual
      ~LogHolderImpl() throw ();

      SyncPolicy::Mutex& mutex_() throw();

    private:
      SyncPolicy::Mutex lock_;

    protected:
      LogFlushTraits flush_traits_;
      SavePolicy save_policy_;
      Generics::Time flush_time_;
      CollectorT collector_;
    };

    /** LogHolderLimitedDataAdd
     *    Adapter for loggers with add(DataT).
     *    Store not more than limit records.
     *    On flush write number of skipped records to Logging::Logger
     */
    template <
      typename LogTraitsType,
      typename SavePolicy = DefaultSavePolicy<LogTraitsType>
    >
    class LogHolderLimitedDataAdd:
      public LogHolderImpl<LogTraitsType, SavePolicy>
    {
    private:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      Logging::Logger_var logger_;
      const size_t limit_;
      size_t count_;
      size_t skipped_;

    public:
      LogHolderLimitedDataAdd(
        Logging::Logger* logger,
        size_t limit,
        const LogFlushTraits& flush_traits,
        const SavePolicy& save_policy = SavePolicy())
        throw(eh::Exception);

      void add_record(
        typename LogHolderImpl<LogTraitsType, SavePolicy>::
          CollectorT::DataT data)
        throw(eh::Exception);

      Generics::Time flush_if_required(
        const Generics::Time& now) throw(eh::Exception);

      ~LogHolderLimitedDataAdd() throw();
    protected:
      void add_record_i_(
        typename LogHolderImpl<LogTraitsType, SavePolicy>::
          CollectorT::DataT&& data)
        throw(eh::Exception);
    };

    class CompositeLogHolder:
      public virtual LogHolder,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      void add_child_log_holder(LogHolder* log_holder) throw(eh::Exception);

      virtual Generics::Time flush_if_required(
        const Generics::Time& now = Generics::Time::get_time_of_day()) throw(eh::Exception);

    protected:
      virtual
      ~CompositeLogHolder() throw () = default;

    private:
      typedef Sync::Policy::PosixThread SyncPolicy;
      typedef std::list<LogHolder_var> LogHolderList;

      SyncPolicy::Mutex lock_;
      LogHolderList child_log_holders_;
    };

    template <
      typename LogTraitsType,
      typename SavePolicy = DefaultSavePolicy<LogTraitsType> >
    class LogHolderPoolBase:
      public virtual LogHolder,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      virtual Generics::Time flush_if_required(
        const Generics::Time& now) throw(eh::Exception);

    protected:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      class ContainerHolder;
      struct LogHolderPoolObject;

      typedef ReferenceCounting::SmartPtr<ContainerHolder> ContainerHolder_var;
      typedef Generics::LastPtr<ContainerHolder> LastContainerHolder_var;
      typedef Sync::Policy::PosixThreadRW SyncPolicy;
      typedef ReferenceCounting::SmartPtr<LogHolderPoolObject> LogHolderPoolObject_var;
      typedef std::list<LogHolderPoolObject_var> LogHolderList;

      class PoolObjectBase: public ReferenceCounting::AtomicImpl
      {
      protected:
        PoolObjectBase(const ContainerHolder_var& container_holder)
          : container_holder_(container_holder)
        {
          container_holder_->get_object(holders_);
        }

      protected:
        virtual
        ~PoolObjectBase() throw()
        {
          try
          {
            container_holder_->release_object(holders_);
          }
          catch (const eh::Exception& e)
          {
            std::cerr << "PoolObjectBase::~PoolObjectBase<'" <<
              LogTraitsType::log_base_name() << "'>(): " <<
              "eh::Exception caught: " << e.what() << std::endl;
          }
        }

      protected:
        ContainerHolder_var container_holder_;
        LogHolderList holders_;
      };

    protected:
      LogHolderPoolBase(
        const LogFlushTraits& flush_traits,
        const SavePolicy& save_policy = SavePolicy())
        throw(eh::Exception);

      virtual
      ~LogHolderPoolBase() throw ();

      ContainerHolder_var
      get_container_holder_() const throw(eh::Exception);

    protected:
      const LogFlushTraits flush_traits_;
      SavePolicy save_policy_;
      mutable SyncPolicy::Mutex lock_;
      ContainerHolder_var container_holder_;
      Generics::Time flush_time_;
    };

    template <
      typename LogTraitsType,
      typename SavePolicy = DefaultSavePolicy<LogTraitsType> >
    class LogHolderPool:
      public LogHolderPoolBase<LogTraitsType, SavePolicy>
    {
    public:
      class PoolObject;

      typedef typename LogTraitsType::CollectorType CollectorT;
      typedef LogHolderPoolBase<LogTraitsType, SavePolicy> Base;
      typedef ReferenceCounting::SmartPtr<PoolObject> PoolObject_var;

      class PoolObject: public Base::PoolObjectBase
      {
      public:
        template<typename Mediator>
        void
        add_record(
          typename CollectorT::KeyT key,
          Mediator data);

      protected:
        friend PoolObject_var LogHolderPool::get_object();

        PoolObject(const typename Base::ContainerHolder_var& container_holder)
          : Base::PoolObjectBase(container_holder)
        {}

        virtual
        ~PoolObject() throw () = default;
      };

      LogHolderPool(
        const LogFlushTraits& flush_traits,
        const SavePolicy& save_policy = SavePolicy())
        throw(eh::Exception);

      template<typename Mediator>
      void
      add_record(
        typename CollectorT::KeyT key,
        Mediator data);

      PoolObject_var
      get_object() throw(eh::Exception);

    protected:
      virtual ~LogHolderPool() throw() {}
    };

    template <
      typename LogTraitsType,
      typename SavePolicy = DefaultSavePolicy<LogTraitsType> >
    class LogHolderPoolData:
      public LogHolderPoolBase<LogTraitsType, SavePolicy>
    {
    public:
      class PoolObject;

      typedef typename LogTraitsType::CollectorType CollectorT;
      typedef LogHolderPoolBase<LogTraitsType, SavePolicy> Base;
      typedef ReferenceCounting::SmartPtr<PoolObject> PoolObject_var;

      class PoolObject: public Base::PoolObjectBase
      {
      public:
        void
        add_record(typename CollectorT::DataT data)
        {
          (*this->holders_.begin())->collector.add(std::move(data));
        }

      protected:
        friend PoolObject_var LogHolderPoolData::get_object();

        PoolObject(const typename Base::ContainerHolder_var& container_holder)
          : Base::PoolObjectBase(container_holder)
        {}

        virtual
        ~PoolObject() throw () = default;
      };

      LogHolderPoolData(
        const LogFlushTraits& flush_traits,
        const SavePolicy& save_policy = SavePolicy())
        throw(eh::Exception)
        : LogHolderPoolBase<LogTraitsType, SavePolicy>(flush_traits, save_policy)
      {}

      void
      add_record(typename CollectorT::DataT data)
      {
        PoolObject_var pool_object = get_object();
        pool_object->add_record(std::move(data));
      }

      PoolObject_var
      get_object()
      {
        return new PoolObject(this->get_container_holder_());
      }

    protected:
      virtual
      ~LogHolderPoolData() throw () = default;
    };
  }
}

#include "LogHolder.tpp"

#endif /*LOGCOMMONS_LOGHOLDER_HPP*/
