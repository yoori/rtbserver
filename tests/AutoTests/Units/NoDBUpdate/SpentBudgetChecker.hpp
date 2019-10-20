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


#ifndef _AUTOTEST__SPENTBUDGETCHECKER_
#define _AUTOTEST__SPENTBUDGETCHECKER_
 
#include <tests/AutoTests/Commons/Common.hpp>

namespace AutoTest
{
  /*
   * @class SpentBudgetChecker
   * @brief Check CCG budget reaching.
   */
  class SpentBudgetChecker : public AutoTest::Checker
  {
  public:

    /**
     * @brief Constructor
     *
     * @param test.
     * @param check request.
     * @param CCG ID.
     * @param expected CC ID.
     */
    SpentBudgetChecker(
      BaseUnit* test,
      const NSLookupRequest& request,
      unsigned long ccgid,
      unsigned long ccid,
      const Generics::Time& deadline = Generics::Time::ZERO);

    /**
     * @brief Destructor
     */
    ~SpentBudgetChecker() throw();

    /**
     * @brief check
     */
    bool check(bool throw_error = true)
      throw (AutoTest::CheckFailed, eh::Exception);

  private:
    BaseUnit* test_;          // test
    NSLookupRequest request_; // check 
    unsigned long ccgid_;     // CCG ID
    unsigned long  ccid_;        // expected CC ID
    Generics::Time deadline_;
  };
}; // namespace AutoTest

#endif //_AUTOTEST__SPENTBUDGETCHECKER_
