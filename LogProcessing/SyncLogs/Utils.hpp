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

// @file SyncLogs/Utils.hpp

#ifndef SYNCLOGS_UTILS_HPP_INCLUDED
#define SYNCLOGS_UTILS_HPP_INCLUDED

#include <string>
#include <list>
#include <algorithm>

#include <eh/Exception.hpp>
#include <Commons/MessagePacker.hpp>

#include "RouteProcessor.hpp"

namespace AdServer
{
  namespace LogProcessing
  {
    struct InterruptCallback
    {
      virtual ~InterruptCallback() throw() {}

      virtual bool interrupt() throw(eh::Exception) = 0;
    };

    typedef std::list<std::string> StringList;

    class LocalInterfaceChecker
    {
    public:
      LocalInterfaceChecker(const std::string& host_name) throw ()
        : host_name_(host_name)
      {}

      bool
      check_host_name(const std::string& host_name) const throw();
    private:
      const std::string& host_name_;
    };

    namespace Utils
    {
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
      DECLARE_EXCEPTION(NotFound, eh::DescriptiveException);

      struct UnlinkException : public eh::DescriptiveException
      {
        UnlinkException(const char* descr) throw();

        ~UnlinkException() throw() {};

        std::string file_name;
      };

      // Policies for log errors pool

      class CellsKey;

      struct MessageOut
      {
        static void
        output(
          Commons::MessageBuf& out,
          const String::SubString& message_prefix,
          const String::SubString& message)
          throw ()
        {
          if (out.size() == 0)
          {
            out.append(message_prefix.data(), message_prefix.size());
          }
          else
          {
            out.append(",", 1);
          }
          out.append(message.data(), message.size());
        }

        static void
        log(
          Logging::Logger* logger,
          const CellsKey* key,
          const String::SubString& buffer,
          std::size_t max_size,
          std::size_t errors_count,
          unsigned long severity,
          const char* aspect,
          const char* code);
      };

      class CellsKey : public ReferenceCounting::AtomicImpl
      {
      public:

        /**
           Adder define interface to put custom key into generic MessagePacker
         */
        class Adder
        {
          typedef Commons::GenericMessagePacker<CellsKey, MessageOut> Packer;
        public:
          Adder(Packer& packer) throw ()
            : packer_(packer)
          {}

         /**
          * @param src_host The host that contain files for SyncLogs processing
          * @param dst_host The destination host for processing files
          * @param src_path The path to processing files
          * @param message_prefix The text is put in the begin of final message
          * @param message The error description and so on
          */
          void
          operator ()(
            const String::SubString& src_host,
            const String::SubString& dst_host,
            const String::SubString& src_path,
            const String::SubString& code,
            const String::SubString& message_prefix,
            const String::SubString& message)
            throw (eh::Exception);

          /**
           * For add_stream
           */
          Packer::PoolStream_var
          operator ()(
            const String::SubString& src_host,
            const String::SubString& dst_host,
            const String::SubString& src_path,
            const String::SubString& code,
            const String::SubString& message_prefix)
            throw (eh::Exception);

        private:
          Packer& packer_;
        };

        CellsKey(
          const String::SubString& src_host_val,
          const String::SubString& dst_host_val,
          const String::SubString& src_path_val,
          const String::SubString& code_val);

        std::size_t
        hash() const throw ();

        bool
        operator ==(const CellsKey& right) const throw ();

        const std::string&
        code() const throw ()
        {
          return code_;
        }

      private:
        ~CellsKey() throw () {}

        std::size_t
        calc_hash_() const throw ();

        const std::string src_host_;
        const std::string dst_host_;
        const std::string src_path_;
        const std::string code_;
        const std::size_t hash_value_;
      };
      typedef ReferenceCounting::SmartPtr<CellsKey> CellsKey_var;

      typedef Commons::MessagePacker<CellsKey, MessageOut> ErrorPool;

      /**
       * This processor do only one, dump all cells to logger
       */
      class LogsRouteProcessorFlusher:
        public RouteProcessor, // TO FIX, it isn't RouteProcessor
        public ReferenceCounting::AtomicImpl
      {
      public:
        /**
         * @param error_logger Must be not zero!
         */
        LogsRouteProcessorFlusher(
          ErrorPool* error_logger)
          throw ();

        /**
         * Dump errors pool to logger
         */
        virtual void
        process() throw ();

      private:
        ErrorPool* error_logger_;
      };

      unsigned int find_host_num(
        const char *file_name,
        unsigned int dest_hosts_size)
        throw(Exception, eh::Exception);

      void unlink_file(const char* name)
        throw(NotFound, UnlinkException);

      int run_cmd(
        const char* cmdline,
        bool read_output,
        std::string& output_str,
        InterruptCallback* interrupter = 0)
        throw(eh::Exception);
    }
  }
}

// Inlines implementation
namespace AdServer
{
  namespace LogProcessing
  {

    namespace Utils
    {

      //
      // MessageOut class
      //
      inline void
      MessageOut::log(
        Logging::Logger* logger,
        const CellsKey* key,
        const String::SubString& buffer,
        std::size_t max_size,
        std::size_t errors_count,
        unsigned long severity,
        const char* aspect,
        const char* /*code*/)
      {
        logger->stream(severity,
                       aspect,
                       key->code().c_str())
          << "Aggregate " << errors_count << " errors, descriptions: "
          << buffer
          << (buffer.size() < max_size ? "" : "...[cut]...");
      }

      //
      // CellsKey class
      //
      inline void
      CellsKey::Adder::operator ()(
        const String::SubString& src_host,
        const String::SubString& dst_host,
        const String::SubString& src_path,
        const String::SubString& code,
        const String::SubString& message_prefix,
        const String::SubString& message)
        throw (eh::Exception)
      {
        CellsKey_var key(new CellsKey(src_host, dst_host, src_path, code));
        packer_.add(key, message_prefix, message);
      }

      inline CellsKey::Adder::Packer::PoolStream_var
      CellsKey::Adder::operator ()(
        const String::SubString& src_host,
        const String::SubString& dst_host,
        const String::SubString& src_path,
        const String::SubString& code,
        const String::SubString& message_prefix)
        throw (eh::Exception)
      {
        CellsKey_var key(new CellsKey(src_host, dst_host, src_path, code));
        return packer_.add_stream(key, message_prefix);
      }

      inline
      CellsKey::CellsKey(
        const String::SubString& src_host_val,
        const String::SubString& dst_host_val,
        const String::SubString& src_path_val,
        const String::SubString& code_val)
        : src_host_(src_host_val.data(), src_host_val.size()),
          dst_host_(dst_host_val.data(), dst_host_val.size()),
          src_path_(src_path_val.data(), src_path_val.size()),
          code_(code_val.data(), code_val.size()),
          hash_value_(calc_hash_())
      {}

      inline std::size_t
      CellsKey::hash() const throw ()
      {
        return hash_value_;
      }

      inline std::size_t
      CellsKey::calc_hash_() const throw ()
      {
        std::size_t hash;
        {
          Generics::Murmur64Hash hasher(hash);
          hash_add(hasher, src_host_);
          hash_add(hasher, dst_host_);
          hash_add(hasher, src_path_);
          hash_add(hasher, code_);
        }
        return hash;
      }

      inline bool
      CellsKey::operator ==(const CellsKey& right) const throw ()
      {
        return src_host_ == right.src_host_ &&
          dst_host_ == right.dst_host_ &&
          src_path_ == right.src_path_ &&
          code_ == right.code_;
      }

    }


    inline
    bool
    LocalInterfaceChecker::check_host_name(const std::string& host_name) const
      throw ()
    {
      return host_name == host_name_;
    }

    namespace Utils
    {
      inline
      UnlinkException::UnlinkException(const char* descr) throw()
        : eh::DescriptiveException(descr)
      {}

      //
      // LogsRouteProcessorFlusher class
      //
      inline
      LogsRouteProcessorFlusher::LogsRouteProcessorFlusher(
        ErrorPool* error_logger)
        throw ()
        : error_logger_(error_logger)
      {}

      inline void
      LogsRouteProcessorFlusher::process() throw ()
      {
        error_logger_->dump(Logging::Logger::ERROR, "SyncLogs");
      }
    }
  }
}

#endif // SYNCLOGS_UTILS_HPP_INCLUDED
