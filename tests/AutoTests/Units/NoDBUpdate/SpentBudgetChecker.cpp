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


#include "SpentBudgetChecker.hpp"
 
namespace AutoTest
{
  SpentBudgetChecker::SpentBudgetChecker(
    BaseUnit* test,
    const NSLookupRequest& request,
    unsigned long ccgid,
    unsigned long ccid,
    const Generics::Time& deadline) :
    test_(test),
    request_(request),
    ccgid_(ccgid),
    ccid_(ccid),
    deadline_(deadline)
  {}


  SpentBudgetChecker::~SpentBudgetChecker()
    throw()
  {}

  bool
  SpentBudgetChecker::check(
    bool throw_error)
    throw (AutoTest::CheckFailed, eh::Exception)
  {
    CampaignChecker campaign_checker(test_, ccgid_,
      CampaignChecker::Expected().eval_status(ccid_ == 0? "I": "A"));

    SelectedCreativeChecker selected_creative_checker(
      AdClient::create_user(test_), request_, ccid_);

    try
    {
      return 
        and_checker(
          wait_checker(
            deadline_ != Generics::Time::ZERO
              ? and_checker(
                  throw_checker(TimeLessChecker(deadline_)),
                  campaign_checker)
              : and_checker(campaign_checker)),
          SelectedCreativeChecker(
            AdClient::create_user(test_),
            request_,
            ccid_)).check(throw_error);
      
    }
    catch (const TimeLessChecker::TimeLessCheckFailed&)
    {
      throw;
    }
    catch (const AutoTest::CheckFailed& e)
    {
      if (deadline_ != Generics::Time::ZERO)
      {
        AutoTest::TimeLessChecker(deadline_).check(true);
      }
      throw;
    }
    return false;
  }
}; //namespace AutoTest
