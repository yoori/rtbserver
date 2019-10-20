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

#ifndef USERINFOSVCS_USERBINDSERVER_CHUNKUTILS_HPP
#define USERINFOSVCS_USERBINDSERVER_CHUNKUTILS_HPP

#include <iostream>
#include <fstream>
#include <sstream>

#include <Generics/Time.hpp>
#include <Generics/DirSelector.hpp>
#include <Generics/Rand.hpp>
#include <String/RegEx.hpp>

namespace AdServer
{
namespace UserInfoSvcs
{
  // path to file name map
  typedef std::map<std::string, std::string> FilePathMap;

  struct BaseChunkSelector
  {
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    BaseChunkSelector(const String::SubString& prefix);

    bool
    match(std::string* date_str, const char* full_path)
      const
      throw ();

  private:
    String::RegEx reg_exp_;
  };

  struct ChunkRemover: public BaseChunkSelector
  {
    ChunkRemover(const String::SubString& prefix);

    bool
    operator ()(const char* full_path, const struct stat&)
      const
      throw (BaseChunkSelector::Exception);
  };

  struct ChunkSelector: public BaseChunkSelector
  {
    struct ChunkFileDescription
    {
      ChunkFileDescription(
        const Generics::Time& max_keep_time)
        : max_time(max_keep_time)
      {}

      Generics::Time max_time;
    };

    /// In really used full_path -> max_keep_time map.
    typedef std::map<std::string, ChunkFileDescription>
      ChunkFileDescriptionMap;

    ChunkSelector(
      const String::SubString& prefix,
      ChunkFileDescriptionMap& chunk_files);

    bool
    operator ()(const char* full_path, const struct stat&)
      throw (BaseChunkSelector::Exception);

  private:
    /// Reference to container that get result map of files
    ChunkFileDescriptionMap& chunk_files_;
  };
}
}

#endif /*USERINFOSVCS_USERBINDSERVER_CHUNKUTILS_HPP*/
