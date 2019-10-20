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


#ifndef _AUTOTEST_COMMONS_ORM_UTILS_HPP
#define _AUTOTEST_COMMONS_ORM_UTILS_HPP

#include "ORM.hpp"

namespace AutoTest
{
  namespace ORM
  {
    template<typename T>
    void
    Unused(const T& x);
    
    int
    update_(
      std::ostream& strm,
      int counter,
      void* object,
      const ORMObjectMember& var,
      bool set_defaults = false);

    template<typename T>
    void
    setin_(
      DB::Query& query,
      T& var);

    int
    count_(
      int counter,
      void* object,
      const ORMObjectMember& var,
      bool set_defaults = false);

    int
    insert_(
      std::ostream& strm,
      int counter, void* object,
      const ORMObjectMember& var,
      bool set_defaults = false);

    void
    put_var_(
      std::ostream& strm,
      int counter,
      void* object,
      const ORMObjectMember& var,
      bool set_defaults = false);

    template<typename T>
    void
    insertin_(
      DB::Query& query,
      T& var);
  }
}

#include "Utils.ipp"

#endif  // _AUTOTEST_COMMONS_ORM_UTILS_HPP
