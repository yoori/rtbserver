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

#ifndef __AUTOTESTS_COMMONS_CHECKER_STATSDIFFCHECKER_HPP
#define __AUTOTESTS_COMMONS_CHECKER_STATSDIFFCHECKER_HPP

#include "Checker.hpp"

namespace AutoTest
{
  /**
   * @class StatsDiffChecker
   * Checks that initial[i] + diff[i] == real[i] for all i
   */
  template<typename StatsType, typename StatsDiffType>
  class StatsDiffChecker: public Checker
  {
  public:
    StatsDiffChecker(
      DBC::IConn& connection,
      const StatsDiffType& diff,
      const StatsType& initial);

    virtual ~StatsDiffChecker() throw();

    bool
    check(
      bool throw_error = true)
      throw (CheckFailed, eh::Exception);

  protected:
    DBC::IConn& connection_;
    StatsDiffType diff_;
    StatsType initial_; 
    StatsType real_;
  };

  /**
   * @class StatsEachDiffChecker
   * Checks that initial[i] + diff == real[i] for all i
   */
  template<typename StatsType, typename StatsDiffType>
  class StatsEachDiffChecker: public Checker
  {
  public:
    StatsEachDiffChecker(
      DBC::IConn& connection,
      const StatsDiffType& diff,
      const StatsType& initial);

    virtual ~StatsEachDiffChecker() throw();

    bool
    check(
      bool throw_error = true)
      throw (CheckFailed, eh::Exception);

  protected:
    DBC::IConn& connection_;
    StatsDiffType diff_;
    StatsType initial_;
    StatsType real_;
  };

  // helper functions
  template<typename StatsType, typename StatsDiffType>
  StatsDiffChecker<StatsType, StatsDiffType>
  stats_diff_checker(
    DBC::IConn& connection,
    const StatsDiffType& diff,
    const StatsType& initial);

  template<typename StatsType, typename Diff>
  StatsDiffChecker<StatsType, std::list<Diff> >
  stats_diff_checker(
    DBC::IConn& connection,
    const std::initializer_list<Diff>& diff,
    const StatsType& initial);

  template<typename StatsType, typename StatsDiffType>
  StatsEachDiffChecker<StatsType, StatsDiffType>
  stats_each_diff_checker(
    DBC::IConn& connection,
    const StatsDiffType& diff,
    const StatsType& initial);
} //namespace AutoTest

#include "StatsDiffChecker.tpp"

#endif  // __AUTOTESTS_COMMONS_CHECKER_STATSDIFFCHECKER_HPP
