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


#ifndef _AUTOTESTS_COMMONS_REQUEST_DEBUGSIZEPARAM_HPP
#define _AUTOTESTS_COMMONS_REQUEST_DEBUGSIZEPARAM_HPP

#include "BaseRequest.hpp"

namespace AutoTest
{
  /**
   * @brief debug size complex param
   *
   * Allows to set debug size for indicated/all  banner(s).
   */
  template<typename Request>
  class DebugSizeParam:
    public ComplexParam<Request, unsigned long>
  {
    typedef ComplexParam<Request, unsigned long> Base;

  public:
    template<class T>
    DebugSizeParam(
      Request* request,
      const char* name,
      const T& defs,
      bool set_defs = true);

    DebugSizeParam(Request* request, const char* name);

    DebugSizeParam(Request* request, const DebugSizeParam& other);

    template<class T>
    DebugSizeParam& operator= (const T& val);

    virtual bool
    print(
      std::ostream& out,
      const char* prefix,
      const char* eql) const;

    virtual ~DebugSizeParam() throw();
  };
};

#include "DebugSizeParam.tpp"

#endif  // _AUTOTESTS_COMMONS_REQUEST_DEBUGSIZEPARAM_HPP

