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

#ifndef USERINFOMANAGER_USEROPERATIONLOADER_HPP
#define USERINFOMANAGER_USEROPERATIONLOADER_HPP

#include <eh/Exception.hpp>

#include <Generics/TaskRunner.hpp>
#include <Generics/Values.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <Logger/ActiveObjectCallback.hpp>

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Logger/Logger.hpp>

#include <Generics/TaskRunner.hpp>
#include <Generics/Values.hpp>
#include <Generics/CompositeActiveObject.hpp>

#include "UserOperationProcessor.hpp"

namespace AdServer
{
  namespace UserInfoSvcs
  {
    /* BaseOperationRecordFetcher class */
    class BaseOperationRecordFetcher: public LogProcessing::FileProcessor,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      typedef std::set<unsigned long> ChunkIdSet;

      BaseOperationRecordFetcher(
        Generics::ActiveObjectCallback* callback,
        UserOperationProcessor* user_operation_processor,
        const char* folder,
        const char* unprocessed_folder,
        const char* file_prefix,
        const ChunkIdSet& chunk_ids,
        Generics::ActiveObject* interrupter)
        throw();

      virtual void
      process(LogProcessing::FileReceiver::FileGuard* file_ptr) throw ();

    protected:
      virtual
      ~BaseOperationRecordFetcher() throw ()
      {}

      virtual void
      read_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception) = 0;

    private:
      void
      file_move_back_to_input_dir_(
        const AdServer::LogProcessing::LogFileNameInfo& info,
        const char* file_name) throw (eh::Exception);

    protected:
      UserOperationProcessor_var user_operation_processor_;

    private:
      Generics::ActiveObjectCallback_var log_errors_;

      // check configuration
      const std::string DIR_;
      const std::string unprocessed_dir_;
      const std::string file_prefix_;

      const ChunkIdSet chunk_ids_;
      Generics::ActiveObject_var interrupter_;
    };

    typedef ReferenceCounting::AssertPtr<BaseOperationRecordFetcher>::Ptr
      BaseOperationRecordFetcher_var;

    /* ExternalOperationRecordFetcher class */
    class ExternalOperationRecordFetcher:
      public BaseOperationRecordFetcher
    {
    public:
      ExternalOperationRecordFetcher(
        Generics::ActiveObjectCallback* callback,
        UserOperationProcessor* user_operation_processor,
        const char* folder,
        const char* unprocessed_folder,
        const char* file_prefix,
        const ChunkIdSet& chunk_ids,
        Generics::ActiveObject* interrupter)
        throw();

    protected:
      virtual
      ~ExternalOperationRecordFetcher() throw ()
      {}

      virtual void
      read_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception);

      void
      read_add_audience_operation_(
        const Generics::MemBuf& mem_buf)
        throw(eh::Exception);
      
      void
      read_remove_audience_operation_(
        const Generics::MemBuf& mem_buf)
        throw(eh::Exception);
    };
    typedef ReferenceCounting::AssertPtr<ExternalOperationRecordFetcher>::Ptr
      ExternalOperationRecordFetcher_var;

    /* InternalOperationRecordFetcher class */
    class InternalOperationRecordFetcher:
      public BaseOperationRecordFetcher
    {
    public:
      InternalOperationRecordFetcher(
        Generics::ActiveObjectCallback* callback,
        UserOperationProcessor* user_operation_processor,
        const char* folder,
        const char* unprocessed_folder,
        const char* file_prefix,
        const ChunkIdSet& chunk_ids,
        Generics::ActiveObject* interrupter)
        throw();

    protected:
      virtual
      ~InternalOperationRecordFetcher() throw ()
      {}

      virtual void
      read_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception);

      void read_fraud_operation_(
        const Generics::MemBuf& mem_buf)
        throw(eh::Exception);

      void read_match_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception);

      void read_merge_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception);

      void read_fc_update_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception);

      void read_fc_confirm_operation_(
        Generics::SmartMemBuf* smart_mem_buf)
        throw(eh::Exception);

      void
      read_add_audience_operation_(
        const Generics::MemBuf& mem_buf)
        throw(eh::Exception);

      void
      read_remove_audience_operation_(
        const Generics::MemBuf& mem_buf)
        throw(eh::Exception);
    };
    typedef ReferenceCounting::AssertPtr<InternalOperationRecordFetcher>::Ptr
      InternalOperationRecordFetcher_var;

    /* InternalUserOperationLoader class */
    class InternalUserOperationLoader:
      public virtual ReferenceCounting::AtomicImpl,
      public virtual Generics::CompositeActiveObject
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      InternalUserOperationLoader(
        Generics::ActiveObjectCallback* callback,
        UserOperationProcessor* user_operation_processor,
        const char* operation_file_in_dir,
        const char* unprocessed_dir,
        const char* file_prefix,
        const BaseOperationRecordFetcher::ChunkIdSet& chunk_ids,
        const Generics::Time& check_period,
        std::size_t threads_count)
        throw (Exception);

    protected:
      virtual
      ~InternalUserOperationLoader() throw();

    private:
      Generics::ActiveObjectCallback_var log_errors_callback_;
      LogProcessing::FileThreadProcessor_var file_thread_processor_;
      InternalOperationRecordFetcher_var operation_fetcher_;
      std::string unprocessed_dir_;
    };

    typedef ReferenceCounting::SmartPtr<InternalUserOperationLoader>
      InternalUserOperationLoader_var;

    /* ExternalUserOperationLoader class */
    class ExternalUserOperationLoader:
      public virtual ReferenceCounting::AtomicImpl,
      public virtual Generics::CompositeActiveObject
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      ExternalUserOperationLoader(
        Generics::ActiveObjectCallback* callback,
        UserOperationProcessor* user_operation_processor,
        const char* operation_file_in_dir,
        const char* unprocessed_dir,
        const char* file_prefix,
        const BaseOperationRecordFetcher::ChunkIdSet& chunk_ids,
        const Generics::Time& check_period,
        std::size_t threads_count)
        throw (Exception);

    protected:
      virtual
      ~ExternalUserOperationLoader() throw();

    private:
      Generics::ActiveObjectCallback_var log_errors_callback_;
      LogProcessing::FileThreadProcessor_var file_thread_processor_;
      ExternalOperationRecordFetcher_var operation_fetcher_;
      std::string unprocessed_dir_;
    };

    typedef ReferenceCounting::SmartPtr<ExternalUserOperationLoader>
      ExternalUserOperationLoader_var;
  }
}

#endif /*USERINFOMANAGER_USEROPERATIONLOADER_HPP*/
