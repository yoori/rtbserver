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

#ifndef COMMONS_FILEMANIP_HPP_
#define COMMONS_FILEMANIP_HPP_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string>
#include <eh/Exception.hpp>
#include <eh/Errno.hpp>
#include <String/SubString.hpp>

namespace AdServer
{
  namespace FileManip
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    bool
    dir_exists(const String::SubString& path) throw();

    bool
    file_exists(const String::SubString& path) throw();

    void
    rename(const String::SubString& src,
      const String::SubString& dst,
      bool ignore_non_existing)
      throw(eh::Exception);
  }
}

namespace AdServer
{
namespace FileManip
{
  inline bool
  dir_exists(const String::SubString& path) throw()
  {
    struct stat info;

    if(::stat(path.str().c_str(), &info) != 0)
    {
      return false;
    }
    else if(info.st_mode & S_IFDIR)
    {
      return true;
    }

    return false;
  }

  inline bool
  file_exists(const String::SubString& path) throw()
  {
    struct stat info;

    if(::stat(path.str().c_str(), &info) != 0)
    {
      return false;
    }
    else if(info.st_mode & S_IFREG)
    {
      return true;
    }

    return false;
  }

  void
  rename(const String::SubString& src,
    const String::SubString& dst,
    bool ignore_non_existing)
    throw(eh::Exception)
  {
    static const char* FUN = "rename()";

    if(std::rename(src.str().c_str(), dst.str().c_str()))
    {
      if(!ignore_non_existing || errno != ENOENT)
      {
        eh::throw_errno_exception<Exception>(
          FUN,
          ": failed to rename file '",
          src.str().c_str(),
          "' to '",
          dst.str().c_str(),
          "'");
      }
    }
  }
}
}


#endif /*COMMONS_FILEMANIP_HPP_*/
