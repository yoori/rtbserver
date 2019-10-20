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

#include <sstream>
#include <algorithm>
#include <HTTP/UrlAddress.hpp>
#include <tests/AutoTests/Commons/Common.hpp>

namespace AutoTest
{
  namespace Cookie
  {

    static const unsigned long ONE_YEAR = 365 * 24 * 60 * 60;

    /**
     * @class CookieDomain
     */
    struct CookieIsNotDomain : std::binary_function <HTTP::PersistentCookieDef, std::string, bool>
    {
      bool
      operator() (
        const HTTP::PersistentCookieDef& cookie,
        const std::string& domain) const
      {
        return cookie.secure || domain != cookie.domain;
      }
    };

    /**
     * @class CookieName
     */
    struct CookieName : std::binary_function <HTTP::PersistentCookieDef, std::string, bool>
    {
      bool
      operator() (
        const HTTP::PersistentCookieDef& cookie,
        const std::string& name) const
      {
        return name == cookie.name; 
      }
    };

    // UnitCookieList
    
    UnitCookieList::~UnitCookieList() throw()
    { }

    UnitCookieList::UnitCookieList()
      throw(eh::Exception)
    { }

    UnitCookieList::UnitCookieList(
      const UnitCookieList& src)
      throw(eh::Exception)
      : ClientCookieFacility(src)
    { }

    UnitCookieList::UnitCookieList(
      const UnitCookieList& src,
      const std::string& domain)
      throw(eh::Exception) :
      ClientCookieFacility(src)
    {
      expire_();
      
      remove_if(
        std::bind2nd(
          CookieIsNotDomain(),
          domain));
    }

    bool
    UnitCookieList::find_value(
      const std::string& name,
      std::string& value) const
      throw(eh::Exception)
    {
      const_iterator it = std::find_if(
        begin(), end(),
        std::bind2nd(
          CookieName(), name));

      if (it == end()) return false;
      value = it->value;
      return true;
    }

    void
    UnitCookieList::set_value(
      const std::string& name,
      const std::string& value,
      const std::string& domain,
      const std::string& path,
      bool secure) throw(eh::Exception)
    {
      iterator it = std::find_if(
        begin(), end(),
        std::bind2nd(
          CookieName(), name));
      if ( it == end())
      {
        HTTP::PersistentCookieDef cookie(
          name, value, domain, path,
          Generics::Time::get_time_of_day() + ONE_YEAR,
          secure);
        push_back(cookie);
      }
      else
      {
        it->value = value;
      }
    }

    void
    UnitCookieList::remove_cookie(
      const std::string& name) throw(eh::Exception)
    {
      remove_if(
        std::bind2nd(
          CookieName(), name));
    }

    bool
    UnitCookieList::find_cookie(
      const std::string& name,
      HTTP::CookieDef& cookie)
      throw(eh::Exception)
    {
      iterator it = std::find_if(
        begin(), end(),
        std::bind2nd(
          CookieName(), name));

      if ( it  != end() )
      {
        cookie.operator=(*it);
      }

      return it != end();
    }

  } //namespace Cookie
} //namespace AutoTest

