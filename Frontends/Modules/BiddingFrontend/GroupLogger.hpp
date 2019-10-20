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
#ifndef BIDDING_FRONTEND_GROUP_LOGGER_HPP_INCLUDED
#define BIDDING_FRONTEND_GROUP_LOGGER_HPP_INCLUDED

#include <string>
#include <iomanip>

#include <eh/Exception.hpp>
#include <Commons/MessagePacker.hpp>

namespace AdServer
{
  namespace Bidding
  {
    // Policies for log errors pool
    class CellsKey;

    struct MessageOut
    {
      static void
      output(
        Commons::MessageBuf& out,
        const String::SubString& /*message_prefix*/,
        const String::SubString& message)
        throw ()
      {
        if (out.size() == 0)
        {
          out.append(message.data(), message.size());
        }
      }

      static void
      log(
        Logging::Logger* logger,
        CellsKey* key,
        const String::SubString& buffer,
        std::size_t /*max_size*/,
        std::size_t errors_count,
        unsigned long severity,
        const char* aspect,
        const char* code);
    };

    class CellsKey: public ReferenceCounting::AtomicImpl
    {
    public:
      /**
         Adder define interface to put custom key into generic MessagePacker
       */
      class Adder
      {
        typedef Commons::GenericMessagePacker<CellsKey, MessageOut> Packer;
      public:
        explicit Adder(Packer& packer) throw ()
          : packer_(packer)
        {}

        void
        operator ()(
          const char* hostname,
          const String::SubString& message,
          const Generics::Time& timeout,
          unsigned long severity,
          const char* aspect,
          const char* code)
          throw (eh::Exception);

      private:
        Packer& packer_;
      };

      CellsKey(
        const char* hostname,
        const Generics::Time& timeout,
        const String::SubString& message,
        unsigned long severity,
        const char* aspect,
        const char* code);

      std::size_t
      hash() const throw ();

      bool
      operator==(const CellsKey& right) const throw ();

      Generics::Time
      timeout() const throw ()
      {
        return timeout_;
      }

      unsigned long
      severity() const throw ()
      {
        return severity_;
      }

      const char*
      aspect() const throw ()
      {
        return aspect_.c_str();
      }

      const char*
      code() const throw ()
      {
        return code_.c_str();
      }

      const std::string&
      hostname() const throw ()
      {
        return hostname_;
      }

      static Generics::Time
      round_timeout(const Generics::Time& timeout) throw ();

    private:
      ~CellsKey() throw () {}

      std::size_t
      calc_hash_() const throw ();

    private:
      const std::string hostname_;
      const std::string message_;
      unsigned long severity_;
      const std::string aspect_;
      const std::string code_;
      const Generics::Time timeout_;
      const std::size_t hash_value_;
    };

    typedef ReferenceCounting::SmartPtr<CellsKey> CellsKey_var;

    class GroupLogger:
      public Logging::LoggerCallbackHolder,
      public Commons::MessagePacker<CellsKey, MessageOut>,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      GroupLogger(
        Logging::Logger* logger,
        const char* message_prefix,
        const char* aspect,
        const char* code) throw (eh::Exception)
        : Logging::LoggerCallbackHolder(logger, message_prefix, aspect, code),
          Commons::MessagePacker<CellsKey, MessageOut>(10240,
            static_cast<Logging::LoggerCallbackHolder*>(this)->logger())
      {}

      GroupLogger*
      group_logger() throw()
      {
        return this;
      }

    protected:
      virtual ~GroupLogger() throw()
      {}
    };

    typedef ReferenceCounting::SmartPtr<GroupLogger> GroupLogger_var;
  }
}

// Inlines implementation
namespace AdServer
{
  namespace Bidding
  {
    inline void
    MessageOut::log(
      Logging::Logger* logger,
      CellsKey* key,
      const String::SubString& buffer,
      std::size_t /*max_size*/,
      std::size_t errors_count,
      unsigned long /*severity*/,
      const char* /*aspect*/,
      const char* /*code*/)
    {
      const Generics::Time& time = key->timeout();
      char buf[256];
      if (time.tv_sec)
      {
        Stream::Buffer<256> os(buf);
        os << ' ' << time.tv_sec;
      }
      else
      {
        Stream::Buffer<256> os(buf);
        os << " 0.";
        os.width(2);
        os << std::setfill('0');
        os << time.tv_usec / 10000;
      }

      Stream::Error ostr;
      if (!key->hostname().empty())
      {
        ostr << "host: " << key->hostname() << ", ";
      }
      ostr << buffer
        << buf << " (sec), " << errors_count << " requests";

      logger->log(ostr.str(),
        key->severity(),
        key->aspect(),
        key->code());
    }

    //
    // CellsKey class
    //
    inline void
    CellsKey::Adder::operator ()(
      const char* hostname,
      const String::SubString& message,
      const Generics::Time& timeout,
      unsigned long severity,
      const char* aspect,
      const char* code)
      throw (eh::Exception)
    {
      CellsKey_var key(new CellsKey(hostname, timeout, message, severity, aspect, code));
      packer_.add(key, String::SubString(), message);
    }

    inline
    CellsKey::CellsKey(
      const char* hostname,
      const Generics::Time& timeout,
      const String::SubString& message,
      unsigned long severity,
      const char* aspect,
      const char* code)
      : hostname_(hostname),
        message_(message.str()),
        severity_(severity),
        aspect_(aspect),
        code_(code),
        timeout_(round_timeout(timeout)),
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
        hash_add(hasher, hostname_);
        hash_add(hasher, message_);
        hash_add(hasher, aspect_);
        hash_add(hasher, code_);
        hash_add(hasher, severity_);
        hash_add(hasher, timeout_);
      }
      return hash;
    }

    inline bool
    CellsKey::operator ==(const CellsKey& right) const throw ()
    {
      return hostname_ == right.hostname_ &&
        message_ == right.message_ &&
        timeout_ == right.timeout_ &&
        severity_ == right.severity_ &&
        code_ == right.code_ && aspect_ == right.aspect_;
    }

    inline Generics::Time
    CellsKey::round_timeout(const Generics::Time& timeout) throw ()
    {
      suseconds_t microseconds = timeout.tv_usec;
      if (timeout.tv_sec)
      {
        return Generics::Time(timeout.tv_sec + (microseconds ? 1 : 0));
      }
      if (microseconds >= 99999)
      {
        const suseconds_t divider = 100000;
        suseconds_t rounded_up = microseconds - 1 + divider -
          (microseconds - 1) % divider;
        if (rounded_up == 1000000)
        {
          return Generics::Time::ONE_SECOND;
        }
        timeval t = {0, rounded_up};
        return Generics::Time(t);
      }
      if (microseconds > 0)
      {
        const suseconds_t divider = 10000;
        suseconds_t rounded_up = microseconds - 1 + divider -
          (microseconds - 1) % divider;
        timeval t = {0, rounded_up};
        return Generics::Time(t);
      }
      return Generics::Time();
    }

  }
}

#endif // BIDDING_FRONTEND_GROUP_LOGGER_HPP_INCLUDED
