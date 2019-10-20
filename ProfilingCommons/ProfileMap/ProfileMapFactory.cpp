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

#include "ProfileMapFactory.hpp"

namespace AdServer
{
namespace ProfilingCommons
{
  class ChunkDirectoryAssigner
  {
  public:
    ChunkDirectoryAssigner(
      ProfileMapFactory::ChunkPathMap& chunks,
      const char* chunks_root,
      const char* chunks_prefix)
      throw()
      : chunks_(chunks),
        chunks_root_(chunks_root),
        chunks_prefix_(chunks_prefix)
    {}

    bool operator()(const char* file_path, const struct stat& /*st*/)
      throw (eh::Exception)
    {
      String::SubString prefix;
      String::SubString chunk_index_str;
      String::SubString file_name_substr(file_path);

      unsigned long chunk_index;

      String::StringManip::Splitter<
        String::AsciiStringManip::SepUnderscore> tokenizer(
          file_name_substr);

      if(tokenizer.get_token(prefix) &&
         tokenizer.get_token(chunk_index_str) &&
         (prefix == chunks_prefix_) &&
         String::StringManip::str_to_int(chunk_index_str, chunk_index))
      {
        chunks_.insert(std::make_pair(chunk_index, chunks_root_ + "/" + file_path));
      }

      return true;
    }

  private:
    ProfileMapFactory::ChunkPathMap& chunks_;
    std::string chunks_root_;
    std::string chunks_prefix_;
  };

  void
  ProfileMapFactory::fetch_chunk_folders(
    ChunkPathMap& chunks,
    const char* chunks_root,
    const char* chunks_prefix)
    throw(eh::Exception)
  {
    ChunkDirectoryAssigner chunk_dir_assigner(chunks, chunks_root, chunks_prefix);

    Generics::DirSelect::directory_selector(
      chunks_root,
      Generics::DirSelect::wrap_functor(chunk_dir_assigner, true));
  }
}
}
