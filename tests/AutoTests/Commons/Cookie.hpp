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

/** $Id$
 * @file Cookie.hpp
 * Http cookie wrapper.
 */
#ifndef __AUTOTESTS_COMMONS_COOKIE_HPP
#define __AUTOTESTS_COMMONS_COOKIE_HPP

#include <HTTP/HTTPCookie.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>


namespace AutoTest
{

  namespace Cookie
  {
    /**
     * @class UnitCookieList
     * @brief Cookies for AutoTest AdClient.
     *
     * It inherits Generics::HTTP::ClientCookieFacility class and represents
     * list of cookies returned to AutoTest adclient by AdServer.
     */
    class UnitCookieList: public HTTP::ClientCookieFacility
    {
    public:

      /**
       * @brief Default constructor.
       */
      UnitCookieList()
        throw(eh::Exception);

      /**
       * @brief Copy constructor.
       * @param src copied object.
       */
      UnitCookieList(const UnitCookieList& src)
        throw(eh::Exception);

      /**
       * @brief Copy & filter cookies.
       * @note Before filtering remove expired cookies.
       * @param src copied object.
       * @param domain cookie domain value to filter cookies.
       */
      UnitCookieList(
        const UnitCookieList& src,
        const std::string& domain)
        throw(eh::Exception);

      /**
       * @brief Destructor.
       */
      virtual ~UnitCookieList() throw();

      /**
       * @brief Finds cookie value.
       *
       * Finds cookie with indicated name and fetchs its value.
       * @param name cookie name.
       * @param value output parameter, represents finded cookie value.
       * @return whether cookie with such name found.
       */
      bool
      find_value(
        const std::string& name,
        std::string& value) const
        throw(eh::Exception);

      /**
       * @brief Sets cookie value.
       *
       * Finds cookie with indicated name and sets new value for it.
       * If cookie with such name wasn't found, creates it
       * and sets its value. Also sets cookie domain and cookie path.
       * @param name cookie name to set.
       * @param value cookie value to set.
       * @param domain cookie domain to set.
       * @param path cookie path to set.
       * @param secure whether set cookie secure.
       */
      void set_value(
        const std::string& name,
        const std::string& value,
        const std::string& domain = "",
        const std::string& path ="/",
        bool secure = false)
        throw(eh::Exception);

      /**
       * @brief Removes cookie.
       *
       * Finds cookie with indicated name and erase it.
       * If there is no cookie with such name do nothing.
       * @param name removing cookie name.
       */
      void
      remove_cookie(
        const std::string& name)
        throw(eh::Exception);

      /**
       * @brief Finds cookie by name.
       * @param name cookie name.
       * @param cookie output parameter, represents finded cookie.
       * @return whether cookie with indicated name found.
       */
      bool
      find_cookie(
        const std::string& name,
        HTTP::CookieDef& cookie)
        throw(eh::Exception);

      using HTTP::ClientCookieFacility::iterator;
      using HTTP::ClientCookieFacility::const_iterator;
      using HTTP::ClientCookieFacility::begin;
      using HTTP::ClientCookieFacility::end;
      using HTTP::ClientCookieFacility::size;
      using HTTP::ClientCookieFacility::empty;
      using HTTP::ClientCookieFacility::remove_if;

    };
    
  } //namespace Cookie
} //namespace AutoTest

#endif //__AUTOTESTS_COMMONS_COOKIE_HPP
