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

// @file Commons/MessagePacker.hpp
#ifndef MESSAGE_PACKER_HPP_INCLUDED
#define MESSAGE_PACKER_HPP_INCLUDED

#include <string>

#include <eh/Exception.hpp>
#include <Generics/Hash.hpp>
#include <ReferenceCounting/HashTable.hpp>
#include <String/SubString.hpp>
#include <Logger/Logger.hpp>

namespace AdServer
{
  namespace Commons
  {

    class MessageBuf
    {
    public:

      MessageBuf(std::size_t size) throw (eh::Exception)
        : buffer_(size),
          size_(0),
          CAPACITY_(size)
      {}

      void
      append(const char* msg, std::size_t size) throw ()
      {
        const std::size_t available_size = CAPACITY_ - size_;
        if (available_size)
        {
          char* str = buffer_.get() + size_;
          std::size_t write_portion = std::min(available_size, size);
          memcpy(str, msg, write_portion);
          size_ += write_portion;
        }
      }

      /**
       * @return buffer size used by user.
       */
      std::size_t
      size() const throw ()
      {
        return size_;
      }

      /**
       * @return really allocated memory by this MessageBuf object.
       */
      std::size_t
      capacity() const throw()
      {
        return CAPACITY_;
      }

      /**
       * @param offset from begin of user data in bytes
       * @return pointer on user data.
       * Constant version.
       */
      const char*
      data(/*std::size_t offset = 0*/) const throw ()
      {
        return buffer_.get();
      }

      void
      clear() throw ()
      {
        size_ = 0;
      }

    private:
      Generics::ArrayChar buffer_;
      //! memory size used for store user structures.
      std::size_t size_;
      //! really allocated bytes.
      const std::size_t CAPACITY_;
    };

    /**
     * Collect and count messages in a few cells with a buffer of fixed size.
     * The cell is addressed by Key values.
     * Dump this cells to logger
     */
    template <typename Key, typename MessageOut>
    class GenericMessagePacker
    {
      typedef ReferenceCounting::SmartPtr<Key> Key_var;
    public:

      /**
       * @param max_message_len The size of buffer that aggregate error
       *   messages
       * @param logger The pointer to the logger to which cells dumping
       */
      GenericMessagePacker(std::size_t max_message_len, Logging::Logger* logger)
        throw ()
        : logger_(ReferenceCounting::add_ref(logger)),
          cell_size_(max_message_len)
      {}

      /**
       * Dump cells to logger
       */
      ~GenericMessagePacker() throw ()
      {
        dump();
      }

      /**
       * Is identify the cell by key and
       * put in the cell for messages. Prefix is placed only in the begin of
       * empty cell. Messages can be aggregate: count number of occurs and
       * length of final message does not exceed a specified length.
       */
      void
      add(
        const Key_var& key,
        const String::SubString& message_prefix,
        const String::SubString& message)
        throw (eh::Exception)
      {
        Sync::PosixGuard guard(table_lock_);
        typename CellsContainer::iterator it = cells_.find(key);
        if (it != cells_.end())
        {
          it->second->add_message(message_prefix, message);
          return;
        }
        Cell_var cell(new Cell(cell_size_));
        it = cells_.insert(typename CellsContainer::value_type(key, cell)).first;
        it->second->add_message(message_prefix, message);
      }

      /**
       * Delegate call to owner logger. Pooling doesn't use.
       */
      bool
      log(const String::SubString& text,
        unsigned long severity = Logging::Logger::INFO,
        const char* aspect = 0, const char* code = 0) throw ()
      {
        return logger_->log(text, severity, aspect, code);
      }

      /**
       * Gets logger trace level.
       * @return current trace level
       */
      unsigned long
      log_level() throw ()
      {
        return logger_->log_level();
      }

      /**
       * Put all cells to logger and clear its
       */
      void
      dump(unsigned long severity = Logging::Logger::ERROR,
        const char* aspect = "",
        const char* code = "")
        throw ();

    private:

      class Cell : public ReferenceCounting::AtomicImpl
      {
        /**
         * RC empty destructor
         */
        ~Cell() throw ();

      public:
        Cell(std::size_t buffer_size) throw ();

        /**
         * If message is first: result message = message_prefix + message
         * else result message += message.
         * The length is truncating to max_message_len
         */
        void
        add_message(
          const String::SubString& message_prefix,
          const String::SubString& message)
          throw ()
        {
          ++counter_;
          MessageOut::output(buffer_, message_prefix, message);
        }

        const String::SubString
        content() const throw ();

        std::size_t
        errors_counter() const throw ();

        void
        reset() throw ();

      private:
        MessageBuf buffer_;

        std::size_t counter_;
      };

      typedef ReferenceCounting::QualPtr<Cell> Cell_var;

    public:
      class PoolStream : public Stream::Error,
        public ReferenceCounting::DefaultImpl<>
      {
      public:
        PoolStream(
          Sync::PosixMutex& table_lock,
          Cell& cell,
          const String::SubString& message_prefix)
          throw ();

      private:
        ~PoolStream() throw ();

        Sync::PosixMutex& table_lock_;
        Cell& cell_;
        const String::SubString& message_prefix_;
      };
      typedef ReferenceCounting::QualPtr<PoolStream> PoolStream_var;

      /**
       * logger->stream() << ...; like feature
       */
      PoolStream_var
      add_stream(
        const Key_var& key,
        const String::SubString& message_prefix)
        throw (eh::Exception);

    private:
      template <class T>
      class HashAdapter
      {
      public:
        HashAdapter(const T& value) throw () : value_(value) {}

        bool
        operator ==(const HashAdapter& src) const throw ()
        {
          return *value_ == *src.value_;
        }

        size_t
        hash() const throw ()
        {
          return value_->hash();
        }

        const T&
        value() const throw ()
        {
          return value_;
        }

      protected:
        T value_;
      };

      void
      dump_(const Key_var& key, Cell& cell, unsigned long severity, const char* aspect,
        const char* code)
        throw ();

      /// Protect container of cells
      Sync::PosixMutex table_lock_;

      typedef ReferenceCounting::HashTable<HashAdapter<Key_var>, Cell_var>
        CellsContainer;
      CellsContainer cells_;

      /// Dump cells to this logger
      Logging::FLogger_var logger_;
      /// Maximum size of message for all created cells.
      const std::size_t cell_size_;
    };

    template <typename Key, typename MessageOut>
    class MessagePacker : public GenericMessagePacker<Key, MessageOut>
    {
    public:
      MessagePacker(std::size_t max_message_len, Logging::Logger* logger)
        throw ()
      : GenericMessagePacker<Key, MessageOut>(max_message_len, logger),
        add_error(*this)
      {}

      typename Key::Adder add_error;
    };

  }
}

// Inlines implementation
namespace AdServer
{
  namespace Commons
  {

    template <typename Key, typename MessageOut>
    inline typename GenericMessagePacker<Key, MessageOut>::PoolStream_var
    GenericMessagePacker<Key, MessageOut>::add_stream(
      const Key_var& key,
      const String::SubString& message_prefix)
      throw (eh::Exception)
    {
      typename CellsContainer::iterator it;
      table_lock_.lock();
      try
      {
        it = cells_.find(key);
        if (it != cells_.end())
        {
          return typename GenericMessagePacker::PoolStream_var(
            new PoolStream(table_lock_, *it->second, message_prefix));
        }
        Cell_var cell(new Cell(cell_size_));
        it = cells_.insert(typename CellsContainer::value_type(key, cell)).first;
        return GenericMessagePacker::PoolStream_var(
          new PoolStream(table_lock_, *it->second, message_prefix));
      }
      catch (...)
      {
        table_lock_.unlock();
        throw;
      }
    }

    template <typename Key, typename MessageOut>
    inline void
    GenericMessagePacker<Key, MessageOut>::dump(
      unsigned long severity,
      const char* aspect,
      const char* code)
      throw ()
    {
      CellsContainer cells;
      {
        Sync::PosixGuard guard(table_lock_);
        cells_.swap(cells);
      }
      for (typename CellsContainer::iterator it(cells.begin());
        it != cells.end(); ++it)
      {
        dump_(it->first.value(), *it->second, severity, aspect, code);
      }
    }

    template <typename Key, typename MessageOut>
    inline void
    GenericMessagePacker<Key, MessageOut>::dump_(
      const Key_var& key,
      Cell& cell,
      unsigned long severity,
      const char* aspect,
      const char* code)
      throw ()
    {
      const String::SubString& cell_content = cell.content();
      if (!cell_content.empty())
      {
        MessageOut::log(logger_,
          key,
          cell_content,
          cell_size_,
          cell.errors_counter(),
          severity,
          aspect,
          code);
        cell.reset();
      }
    }

    //
    // GenericMessagePacker::PoolStream class
    //
    template <typename Key, typename MessageOut>
    inline
    GenericMessagePacker<Key, MessageOut>::PoolStream::PoolStream(
      Sync::PosixMutex& table_lock,
      Cell& cell,
      const String::SubString& message_prefix)
      throw ()
      : table_lock_(table_lock),
        cell_(cell),
        message_prefix_(message_prefix)
    {
    }

    template <typename Key, typename MessageOut>
    inline
    GenericMessagePacker<Key, MessageOut>::PoolStream::~PoolStream() throw ()
    {
      cell_.add_message(message_prefix_, str());
      table_lock_.unlock();
    }

    //
    // GenericMessagePacker::Cell class
    //
    template <typename Key, typename MessageOut>
    inline
    GenericMessagePacker<Key, MessageOut>::Cell::Cell(std::size_t buffer_size) throw ()
      : buffer_(buffer_size),
        counter_(0)
    {}

    template <typename Key, typename MessageOut>
    inline
    GenericMessagePacker<Key, MessageOut>::Cell::~Cell() throw ()
    {}


    template <typename Key, typename MessageOut>
    inline const String::SubString
    GenericMessagePacker<Key, MessageOut>::Cell::content() const throw ()
    {
      return String::SubString(buffer_.data(), buffer_.size());
    }

    template <typename Key, typename MessageOut>
    inline std::size_t
    GenericMessagePacker<Key, MessageOut>::Cell::errors_counter() const throw ()
    {
      return counter_;
    }

    template <typename Key, typename MessageOut>
    inline void
    GenericMessagePacker<Key, MessageOut>::Cell::reset() throw ()
    {
      buffer_.clear();
      counter_ = 0;
    }

  }
}

#endif // MESSAGE_PACKER_HPP_INCLUDED
