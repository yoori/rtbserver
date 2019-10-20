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

#ifndef FRONTENDCOMMONS_APACHEHTTPRESPONSE_HPP
#define FRONTENDCOMMONS_APACHEHTTPRESPONSE_HPP

#include <Apache/Adapters.hpp>

namespace FrontendCommons
{
  //
  // HttpResponse
  //
  class HttpResponse: public Apache::HttpResponse
  {
  public:
    HttpResponse(request_rec* r) throw ();

    void
    add_cookie(const char* value)
      throw (eh::Exception);

    bool
    cookie_installed() const throw ();

  private:
    bool cookie_installed_;
  };
}

namespace FrontendCommons
{
  // HttpResponse
  inline
  HttpResponse::HttpResponse(request_rec* r) throw ()
    : Apache::HttpResponse(r),
      cookie_installed_(false)
  {}

  inline bool
  HttpResponse::cookie_installed() const throw ()
  {
    return cookie_installed_;
  }

  inline void
  HttpResponse::add_cookie(const char* value) throw (eh::Exception)
  {
    cookie_installed_ = true;
    Apache::HttpResponse::add_cookie(value);
  }
}

#endif /*FRONTENDCOMMONS_APACHEHTTPRESPONSE_HPP*/
