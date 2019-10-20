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


#include "Utils.hpp"

namespace AutoTest
{
  namespace ORM
  {
    int
    update_(
      std::ostream& strm,
      int counter,
      void* object,
      const ORMObjectMember& var,
      bool set_defaults)
    {
      if(var.value(object).changed())
      {
        if(counter > 1) strm << ", ";
        strm << var.name << " = :" << counter++;
      }
      else if (var.value(object).is_null() && set_defaults && var.default_value)
      {
        if(counter > 1) strm << ", ";
        strm << var.name << " = " <<  var.default_value; counter++;
      }
      return counter;
    }

    int
    count_(
      int counter,
      void* object,
      const ORMObjectMember& var,
      bool set_defaults)
    {
      if(var.value(object).changed() || !var.value(object).is_null() || (set_defaults && var.default_value))
      {
        return counter + 1;
      }
      return counter;
    }

    int
    insert_(
      std::ostream& strm,
      int counter, void* object,
      const ORMObjectMember& var,
      bool set_defaults)
    {
      if(var.value(object).changed() || !var.value(object).is_null() || (set_defaults && var.default_value))
      {
        if(counter > 1) strm << ", ";
        strm << var.name;
        return counter + 1;
      }
      return counter;
    }

    void
    put_var_(
      std::ostream& strm,
      int counter,
      void* object,
      const ORMObjectMember& var,
      bool set_defaults)
    {
      if (var.value(object).changed() || !var.value(object).is_null())
      {
        strm << ":" << counter << ", ";
      }
      else if(set_defaults && var.default_value)
      {
        strm << var.default_value << ", ";
      }
    }
  }
}


