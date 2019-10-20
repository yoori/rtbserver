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

#pragma once

#include <string>

namespace AdServer
{
  namespace Predictor
  {
    /**
     * File guard, rename to file~ before start processing
     * and rename back after finish processing
     */
    class FileGuard
    {
    public:

      /**
       * @brief Constructor.
       *
       * @param file path
       * @param check or not file exists when finish
       */
      FileGuard(
        const std::string& filepath,
        bool check_exist = false);

      /**
       * @brief Get original file path.
       *
       * @return file path
       */
      const std::string& filepath() const;

      /**
       * @brief Get temporary file path (with ~).
       *
       * @return file path
       */
      const std::string& tmp_filepath() const;
      
      /**
       * @brief Destructor.
       */
      virtual ~FileGuard() throw();
      
    private:
      std::string filepath_;
      std::string tmp_filepath_;
      bool check_exist_;
    };
  }
}
