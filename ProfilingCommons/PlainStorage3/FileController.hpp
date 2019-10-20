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

#ifndef FILECONTROLLER_HPP_
#define FILECONTROLLER_HPP_

#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/Time.hpp>
#include <Generics/MemBuf.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Sync/SyncPolicy.hpp>
#include <Commons/AtomicInt.hpp>

namespace AdServer
{
namespace ProfilingCommons
{
  class FileController: public virtual ReferenceCounting::Interface
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

  public:
    virtual Generics::SmartMemBuf_var
    create_buffer() const throw() = 0;

    virtual int
    open(const char* file_name, int flags, mode_t mode)
      throw(Exception) = 0;

    virtual void
    close(int fd) throw(Exception) = 0;

    virtual ssize_t
    pread(int fd, void* val, unsigned long read_size, unsigned long fd_pos)
      throw(Exception) = 0;

    virtual ssize_t
    read(int fd, void* val, unsigned long read_size)
      throw(Exception) = 0;

    virtual ssize_t
    write(int fd, const void* val, unsigned long write_size)
      throw(Exception) = 0;
  };

  typedef ReferenceCounting::SmartPtr<FileController>
    FileController_var;

  class PosixFileController:
    public FileController,
    public ReferenceCounting::AtomicImpl
  {
  public:
    using FileController::Exception;

    class Stat: public ReferenceCounting::AtomicImpl
    {
      friend class PosixFileController;

    protected:
      virtual void
      add_read_time_(
        const Generics::Time& start,
        const Generics::Time& stop,
        unsigned long size) throw() = 0;

      virtual void
      add_write_time_(
        const Generics::Time& start,
        const Generics::Time& stop,
        unsigned long size) throw() = 0;

      virtual
      ~Stat() throw() = default;
    };

    typedef ReferenceCounting::SmartPtr<Stat> Stat_var;

  public:
    PosixFileController(
      Stat* pread_stat = 0,
      uint64_t min_free_space = 0,
      unsigned long free_space_check_size_period = 0)
      throw();

    virtual Generics::SmartMemBuf_var
    create_buffer() const throw();

    virtual int
    open(const char* file_name, int flags, mode_t mode)
      throw(Exception);

    virtual void
    close(int fd) throw(Exception);

    virtual ssize_t
    pread(int fd_, void* val, unsigned long read_size, unsigned long fd_pos)
      throw(Exception);

    virtual ssize_t
    read(int fd_, void* val, unsigned long read_size)
      throw(Exception);

    virtual ssize_t
    write(int fd_, const void* val, unsigned long write_size)
      throw(Exception);

  protected:
    virtual ~PosixFileController() throw() = default;

    ssize_t
    pread_(int fd_, void* val, unsigned long read_size, unsigned long fd_pos)
      throw(Exception);

    ssize_t
    read_(int fd_, void* val, unsigned long read_size)
      throw(Exception);

    ssize_t
    write_(int fd_, const void* val, unsigned long write_size)
      throw(Exception);

    bool
    control_devices_() const throw();

  protected:
    typedef unsigned long DeviceId;

    struct Device: public ReferenceCounting::AtomicImpl
    {
      Device() throw();

      Algs::AtomicInt write_size_meter;
    };

    typedef ReferenceCounting::SmartPtr<Device>
      Device_var;

    typedef Generics::GnuHashTable<
      Generics::NumericHashAdapter<DeviceId>,
      Device_var>
      DeviceMap;

    typedef Generics::GnuHashTable<
      Generics::NumericHashAdapter<int>,
      Device_var>
      FileMap;

    typedef Sync::Policy::PosixThreadRW
      DevicesSyncPolicy;

    typedef Sync::Policy::PosixThreadRW
      FilesSyncPolicy;

  protected:
    mutable Generics::Allocator::Base_var allocator_;
    const uint64_t min_free_space_;
    const unsigned long free_space_check_size_period_;

    Stat_var stat_;

    DevicesSyncPolicy::Mutex devices_lock_;
    DeviceMap devices_;

    FilesSyncPolicy::Mutex files_lock_;
    FileMap files_;
  };

  class StatImpl: public PosixFileController::Stat
  {
  public:
    struct Counters
    {
      Generics::Time max_time;
      Generics::Time sum_time;
      unsigned long count;

      Counters() throw ();

      Generics::Time
      avg_time() const throw();
    };

  public:
    Counters
    read_counters() const throw();

    Counters
    write_counters() const throw();

    ReferenceCounting::SmartPtr<StatImpl>
    reset() throw();

  protected:
    typedef Sync::Policy::PosixThread SyncPolicy;

  protected:
    virtual
    ~StatImpl() throw() = default;

    virtual void
    add_read_time_(
      const Generics::Time& start,
      const Generics::Time& stop,
      unsigned long)
      throw();

    virtual void
    add_write_time_(
      const Generics::Time& start,
      const Generics::Time& stop,
      unsigned long)
      throw();

    void
    add_time_(
      const Generics::Time& start,
      const Generics::Time& stop,
      SyncPolicy::Mutex& counters_lock,
      Counters& counters)
      throw();

  protected:
    mutable SyncPolicy::Mutex read_counters_lock_;
    Counters read_counters_;
    mutable SyncPolicy::Mutex write_counters_lock_;
    Counters write_counters_;
  };

  typedef ReferenceCounting::SmartPtr<StatImpl> StatImpl_var;

  class SSDFileController:
    public FileController,
    public ReferenceCounting::AtomicImpl
  {
  public:
    using FileController::Exception;

    SSDFileController(
      FileController* delegate_file_controller,
      unsigned long write_block_size = 128*1024)
      throw();

    virtual Generics::SmartMemBuf_var
    create_buffer() const throw();

    virtual int
    open(const char* file_name, int flags, mode_t mode)
      throw(Exception);

    virtual void
    close(int fd) throw(Exception);

    virtual ssize_t
    pread(int fd, void* val, unsigned long read_size, unsigned long fd_pos)
      throw(Exception);

    virtual ssize_t
    read(int fd, void* val, unsigned long read_size)
      throw(Exception);

    virtual ssize_t
    write(int fd, const void* val, unsigned long write_size)
      throw(Exception);

  protected:
    typedef Sync::Policy::PosixThreadRW SyncPolicy;

  protected:
    FileController_var delegate_file_controller_;
    const unsigned long write_block_size_;
    SyncPolicy::Mutex operations_lock_;
  };
}
}

#endif /*FILECONTROLLER_HPP_*/
