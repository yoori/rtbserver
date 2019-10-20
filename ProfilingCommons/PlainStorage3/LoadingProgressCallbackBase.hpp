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

#ifndef LOADINGPROGRESSCALLBACKBASE_HPP
#define LOADINGPROGRESSCALLBACKBASE_HPP

#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>

namespace AdServer
{
namespace ProfilingCommons
{
  class LoadingProgressCallbackBase :
    public ReferenceCounting::AtomicImpl
  {
  public:
    LoadingProgressCallbackBase() throw();

    virtual void post_progress(double value) throw();

    virtual void loading_is_finished() throw();

  protected:
    virtual ~LoadingProgressCallbackBase() throw();
  };
  typedef ReferenceCounting::SmartPtr<LoadingProgressCallbackBase>
    LoadingProgressCallbackBase_var;
}
}

#endif // LOADINGPROGRESSCALLBACKBASE_HPP
