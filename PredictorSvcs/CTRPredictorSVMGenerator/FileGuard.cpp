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

#include "FileGuard.hpp"
#include <Commons/FileManip.hpp>

namespace AdServer
{
  namespace Predictor
  {
    FileGuard::FileGuard(
      const std::string& filepath,
      bool check_exist) :
      filepath_(filepath),
      tmp_filepath_(filepath_ + "~"),
      check_exist_(check_exist)
    {
      AdServer::FileManip::rename(filepath_, tmp_filepath_, !check_exist); 
    }
    
    const std::string& FileGuard::filepath() const
    {
        return filepath_;
    }
      
    const std::string& FileGuard::tmp_filepath() const
    {
      return tmp_filepath_;
    }
      
    
    FileGuard::~FileGuard() throw()
    {
      if (!(check_exist_ && AdServer::FileManip::file_exists(filepath_)))
      {
        AdServer::FileManip::rename(tmp_filepath_, filepath_, true); 
      }
    }
  }
}
 
