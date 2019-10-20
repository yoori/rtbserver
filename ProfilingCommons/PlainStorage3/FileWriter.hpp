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

#ifndef FILEWRITER_HPP
#define FILEWRITER_HPP

#include <eh/Exception.hpp>
#include <Generics/MemBuf.hpp>

#include "FileController.hpp"

namespace AdServer
{
namespace ProfilingCommons
{
  class FileWriter
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    FileWriter(
      const char* file,
      unsigned long buffer_size,
      bool append = false,
      bool disable_caching = false,
      FileController* file_controller = 0)
      throw(Exception);

    FileWriter(
      int fd,
      unsigned long buffer_size,
      FileController* file_controller = 0)
      throw(Exception);

    ~FileWriter() throw();

    // result can be less then read_size only if reached eof
    void
    write(const void* val, unsigned long read_size)
      throw(Exception);

    void
    flush() throw(Exception);

    unsigned long
    size() const throw();

    void
    close() throw(Exception);

  private:
    void
    write_(const void* val, unsigned long read_size)
      throw(Exception);

  private:
    const unsigned long buffer_size_;
    const unsigned long direct_write_min_size_;
    FileController_var file_controller_;

    bool fd_own_;
    int fd_;
    unsigned long fd_pos_;

    Generics::SmartMemBuf_var mem_buf_;
  };
}
}

#endif /*FILEWRITER_HPP*/
