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

#ifndef _FILE_SYSTEM_H_
#define _FILE_SYSTEM_H_

#include <cstdio>
#include <fcntl.h>
#include <ftw.h>

namespace fs
{

  class FileSystem
  {
  public:

    void
    rename(
      const std::string& old_full_name, const std::string& new_full_name)
      throw (std::exception);

    void
    create(const std::string& path, const std::string& file_name)
      throw (std::exception);

    bool
    remove(const std::string& full_path) throw (std::exception);

    void
    mkdir(const std::string& path) throw (std::exception);

    void
    rmrf(const std::string& path) throw (std::exception);

    char*
    getcwd(char* buf, size_t size) throw ();
  };

  FileSystem file_system;

}

namespace fs
{
  char*
  FileSystem::getcwd(char* buf, size_t size) throw ()
  {
    return ::getcwd(buf, size);
  }

  void
  FileSystem::rename(
    const std::string& old_full_name, const std::string& new_full_name)
    throw (std::exception)
  {
    std::rename(old_full_name.c_str(), new_full_name.c_str());
  }

  void
  FileSystem::create(
    const std::string& path, const std::string& file_name)
    throw (std::exception)
  {
    const mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
    const std::string full_name = path + "/" + file_name;
    ::close(::creat(full_name.c_str(), mode));
  }

  bool
  FileSystem::remove(const std::string& full_path)
    throw (std::exception)
  {
    return (::unlink(full_path.c_str()) == 0);
  }

  void
  FileSystem::FileSystem::mkdir(const std::string& path)
    throw (std::exception)
  {
    ::mkdir(path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP);
  }

  int unlink_cb(const char *fpath, const struct stat*, int, struct FTW*)
  {
      int rv = remove(fpath);

      if (rv)
          perror(fpath);

      return rv;
  }

  void
  FileSystem::rmrf(const std::string& path)
    throw (std::exception)
  {
    ::nftw(path.c_str(), unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
  }
}

#endif // _FILE_SYSTEM_H_
