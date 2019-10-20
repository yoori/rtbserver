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

#ifndef AD_SERVER_LOG_COMMONS_FILE_RECEIVER_HPP
#define AD_SERVER_LOG_COMMONS_FILE_RECEIVER_HPP

#include <set>
#include <string>

#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>
#include <Sync/SyncPolicy.hpp>

#include <Logger/Logger.hpp>

namespace AdServer
{
  namespace LogProcessing
  {
    /**
     * External requirements:
     *   - lexicographical order of files MUST be equals to
     *     order by file's timestamp;
     *   - each file MUST be released after processing from
     *     <intermediate_dir> by client of FileReceiver
     *
     * Commit mode:
     *  - add suffix "C" to regular file name as mark for commit mode:
     *    <regular file name>.C
     *  - use file ~<regular file name>.commit[<optional data>] as
     *    confirmation that regular file was uploaded.
     *    It prevents double uploading file by rsync when a connection
     *    problem occurred during upload acknowledgment sending.
     */
    class FileReceiver : public ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      DECLARE_EXCEPTION(Interrupted, Exception);

      struct FileGuard : public ReferenceCounting::AtomicImpl
      {
        friend class FileReceiver;

        FileGuard(const char* file_name) throw (eh::Exception);

        const std::string&
        full_path() const throw ();

        const std::string&
        file_name() const throw ();

        /*
         * return owned file holder to FileReceiver
         */
        void
        revert() throw (eh::Exception);

      protected:
        FileGuard(FileReceiver* file_receiver, const char* file_name)
          throw (eh::Exception);

        virtual
        ~FileGuard() throw ();

      private:
        ReferenceCounting::SmartPtr<FileReceiver> file_receiver_;
        const std::string file_name_;
        const std::string full_path_;
      };

      typedef ReferenceCounting::SmartPtr<FileGuard> FileGuard_var;

      class Callback
      {
      public:
        virtual void
        on_top_changed(const std::string& new_top) throw () = 0;

        virtual
        ~Callback() throw ()
        {}
      };

      typedef std::set<Callback*> Callbacks;

    public:
      /**
       * Constructor
       * Set FileReceiver into undefined state.
       * @param conditional signal condition if FileReceiver moved from
       * empty to non empty state
       */
      FileReceiver(
        const char* intermediate_dir,
        size_t max_files_to_store,
        Generics::ActiveObject* interrupter = 0,
        Logging::Logger* logger = 0)
        throw (eh::Exception);

      FileGuard_var
      get_eldest(std::string& new_top) throw (Interrupted, eh::Exception);

      bool
      fetch_files(
        const char* input_dir,
        const char* prefix)
        throw (Interrupted, eh::Exception);

      /**
       * Return true if FileReceiver is empty or
       * false if state is undefined or non empty
       */
      bool
      empty() const throw ();

      void
      add_callback(Callback* callback) throw (eh::Exception);

      void
      move(const char* full_path) throw (eh::Exception);

    protected:
      virtual
      ~FileReceiver() throw ();

      std::string
      get_eldest_(std::string& new_top) throw (Interrupted, eh::Exception);

      std::string
      get_eldest_i_(std::string& new_top) throw (eh::Exception);

      /**
       * @return top
       */
      const std::string&
      add_file_i_(const std::string& file_name) throw (eh::Exception);

      void
      fetch_intermediate_dir_() throw (Interrupted, eh::Exception);

      void
      file_processed_(const char* file_name) throw ();

      void
      revert_(const std::string& name) throw (eh::Exception);

      void
      fetch_files_handler_(
        const char* full_path,
        const std::string& prefix)
        throw (eh::Exception);

      void
      fetch_intermediate_dir_handler_(
        const char* full_path,
        const std::string&)
        throw (eh::Exception);

      void
      fetch_file_(
        const std::string& name,
        const std::string& full_path)
        throw (eh::Exception);

      virtual void
      dir_select_external_(
        const char* input_dir,
        const char* prefix)
        throw (Interrupted, eh::Exception);

      virtual void
      dir_select_internal_(const char* dir) throw (Interrupted, eh::Exception);

      virtual void
      rename_(
        const char* old_name,
        const char* new_name)
        throw (eh::Exception);

      virtual void
      unlink_(const char* name)
        throw (eh::Exception);

      virtual bool
      access_(const char* name) throw ();

      void
      check_top_changed_(
        const std::string& top,
        const std::string& file_name)
        throw ();

    private:
      typedef std::set<std::string> FileSet;
      typedef Sync::Policy::PosixThread SyncPolicy;

    private:
      const std::string intermediate_dir_;
      const size_t max_files_to_store_;
      Generics::ActiveObject_var interrupter_;
      Logging::Logger_var logger_;

      mutable SyncPolicy::Mutex files_lock_;
      mutable SyncPolicy::Mutex fetch_lock_;

      /// files, which ready for give (present in intermediate_dir_)
      FileSet files_;
      std::string oldest_not_in_files_;

      /// files, which were given and don't processed yet
      FileSet in_progress_files_;

      /* This set using during internal fetch only for prevent the next case:
       *
       * 1. FileGuard was created for 'some_file';
       * 2. internal fetch was started;
       * 3. internal fetch read 'some_file' file entry;
       * 4. destructor of FileGuard for 'some_file' was called;
       * 5. 'some_file' was removed from 'files_' and 'in_progress_files_'
       *      sets and we uses 'processed_files_' set to make decision
       *      for fetch this file or not.
       */
      FileSet processed_files_;

      bool fetch_internal_in_progress_;

      Callbacks callbacks_;
    };

    typedef ReferenceCounting::SmartPtr<FileReceiver>
      FileReceiver_var;

    class FileReceiverInterrupter :
      public ReferenceCounting::AtomicImpl,
      public Generics::SimpleActiveObject
    {
    private:
      virtual
      ~FileReceiverInterrupter() throw ()
      {}
    };

    typedef ReferenceCounting::SmartPtr<FileReceiverInterrupter>
      FileReceiverInterrupter_var;

    class FileProcessor : public virtual ReferenceCounting::Interface
    {
    public:
      virtual void
      process(FileReceiver::FileGuard* file) throw () = 0;

    protected:
      virtual
      ~FileProcessor() throw ()
      {}
    };

    typedef ReferenceCounting::AssertPtr<FileProcessor>::Ptr FileProcessor_var;

    /**
     * file processing via FileProcessor interface in the internal thread pool;
     * periodical fetch files using internal scheduler.
     */
    class FileThreadProcessor :
      public Generics::CompositeActiveObject,
      public ReferenceCounting::AtomicImpl
    {
    public:
      FileThreadProcessor(
        FileProcessor* file_processor,
        Generics::ActiveObjectCallback* callback,
        unsigned threads_number,
        const char* intermediate_dir,
        std::size_t max_files_to_store,
        const char* input_dir,
        const char* prefix,
        const Generics::Time& fetch_period,
        Logging::Logger* logger = nullptr)
        throw (eh::Exception);

      FileThreadProcessor(
        FileProcessor* file_processor,
        Generics::ActiveObjectCallback* callback,
        unsigned threads_number,
        FileReceiver* file_receiver,
        const char* input_dir,
        const char* prefix,
        const Generics::Time& fetch_period,
        Logging::Logger* logger = nullptr)
        throw (eh::Exception);

    protected:
      virtual
      ~FileThreadProcessor() throw ()
      {}

      void
      process_file_() throw();

      void
      fetch_files_() throw ();

    private:
      class FetchGoal : public Generics::Goal,
        public ReferenceCounting::AtomicImpl
      {
      public:
        FetchGoal(FileThreadProcessor& file_thread_processor) throw ()
          : file_thread_processor_(file_thread_processor)
        {}

        virtual
        void
        deliver() throw (eh::Exception)
        {
          file_thread_processor_.fetch_files_();
        }

      protected:
        virtual
        ~FetchGoal() throw ()
        {}

      private:
        FileThreadProcessor& file_thread_processor_;
      };

    protected:
      Generics::Planner_var scheduler_;
      FileReceiver_var file_receiver_;
      std::string input_dir_;
      std::string prefix_;
      const Generics::Time fetch_period_;
      Logging::Logger_var logger_;
    };

    typedef ReferenceCounting::SmartPtr<FileThreadProcessor>
      FileThreadProcessor_var;

    namespace FileReceiverConfig
    {
      std::string
      make_path(const char* log_root, const char* path) throw (eh::Exception);
    }
  }
}

namespace AdServer
{
  namespace LogProcessing
  {
    inline
    const std::string&
    FileReceiver::FileGuard::full_path() const throw ()
    {
      return full_path_;
    }

    inline
    const std::string&
    FileReceiver::FileGuard::file_name() const throw ()
    {
      return file_name_;
    }
  }
}

#endif /*AD_SERVER_LOG_COMMONS_FILE_RECEIVER_HPP*/
