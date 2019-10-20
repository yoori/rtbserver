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

#ifndef COUNT_ACTIVE_OBJECT_HPP
#define COUNT_ACTIVE_OBJECT_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Sync/Condition.hpp>
#include <Generics/ActiveObject.hpp>

#include <Commons/AtomicInt.hpp>

namespace AdServer
{
namespace Commons
{
  /* CountActiveObject
   *   ActiveObject implementation
   *   wait active_count dropping(<= 0) on wait_object
   */
  class CountActiveObject:
    public Generics::SimpleActiveObject,
    public virtual ReferenceCounting::AtomicImpl
  {
  public:
    CountActiveObject() throw ();

    bool
    add_active_count(int inc, bool ignore_state = false) throw ();

    int
    active_count() const throw ();

  protected:
    virtual
    ~CountActiveObject() throw () = default;

    virtual
    bool
    wait_more_() throw ();

  private:
    Algs::AtomicInt active_count_;
  };
  typedef ReferenceCounting::QualPtr<CountActiveObject>
    CountActiveObject_var;
}
}

#endif /*COUNT_ACTIVE_OBJECT_HPP*/
