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

#include <iostream>
#include <Stream/MemoryStream.hpp>

#include "CountActiveObject.hpp"

namespace AdServer
{
namespace Commons
{
  /** CountActiveObject */
  CountActiveObject::CountActiveObject() throw ()
    : active_count_(0)
  {
  }

  int
  CountActiveObject::active_count() const throw ()
  {
    return active_count_;
  }

  bool
  CountActiveObject::wait_more_() throw ()
  {
    return active_count_ > 0;
  }

  bool
  CountActiveObject::add_active_count(int inc, bool ignore_state) throw ()
  {
    if (ignore_state || active() || inc < 0)
    {
      int new_count = active_count_.exchange_and_add(inc) + inc;
      assert(new_count >= 0); // based on current use cases
      if (!active() && new_count <= 0)
      {
        Sync::PosixGuard guard(cond_);
        cond_.broadcast();
      }

      return true;
    }

    return false;
  }
}
}
