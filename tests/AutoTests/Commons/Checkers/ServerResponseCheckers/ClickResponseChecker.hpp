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

#ifndef __AUTOTESTS_COMMONS_CHECKERS_CLICKRESPONSECHECKER_HPP
#define __AUTOTESTS_COMMONS_CHECKERS_CLICKRESPONSECHECKER_HPP

#include <tests/AutoTests/Commons/Checkers/Checker.hpp>
#include <tests/AutoTests/Commons/AdClient.hpp>
#include <String/RegEx.hpp>

namespace AutoTest
{
  /**
   * @class ClickResponseChecker
   * @brief Check click frontend response.
   */
  class ClickResponseChecker: public AutoTest::Checker
  {
  public:

    static const String::RegEx EMPTY_PUB_PRECLICK;
    static const String::RegEx EMPTY_CRCLICK;
    
    /**
     * @brief Constructor.
     *
     * @param client (user).
     * @param expected PUBPRECLICK token.
     * @param expected CRCLICK token.
     */
    ClickResponseChecker(
      AdClient& client,
      const std::string& exp_pub_preclick,
      const std::string& exp_crclick);

    /**
     * @brief Constructor.
     *
     * @param client (user).
     * @param expected PUBPRECLICK regex.
     * @param expected CRCLICK regex.
     */
    ClickResponseChecker(
      AdClient& client,
      const String::RegEx& exp_pub_preclick_regex,
      const String::RegEx& exp_crclick_regex);

    /**
     * @brief Destructor.
     */
    virtual ~ClickResponseChecker() throw();

    /**
     * @brief Check.
     * @param throw on error flag.
     */
    bool
    check(
      bool throw_error = true)
      throw(eh::Exception);

  private:
    AdClient client_; // client(user)
    String::RegEx exp_pub_preclick_regex_; // PUBPRECLICK regex
    String::RegEx exp_crclick_regex_; // CRCLICK regex
  };
} //namespace AutoTest

#endif  // __AUTOTESTS_COMMONS_CHECKERS_CLICKRESPONSECHECKER_HPP
