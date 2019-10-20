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

/* $Id: OptOutLoggingTest.hpp,v 1.3 2008-03-27 08:19:17 artem_nikitin Exp $
  Artem V. Nikitin
  OptOutLogging testing header
 */

#ifndef _UNITTEST__OPTOUTLOGGINGSTEST_
#define _UNITTEST__OPTOUTLOGGINGSTEST_

#include <tests/AutoTests/Commons/Common.hpp>

/**
 * @class OptOutLoggingTest
 * @brief Check OptOutStats logging
 */
class OptOutLoggingTest: public BaseUnit
{

  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::ORM::OptOutStats Stat;
  typedef Stat::Diffs Diff;
  
  struct Request
  {
    AutoTest::Time time;
    const char* operation;
    const char* redirect;
  };

  struct Expected
  {
    AutoTest::Time time;
    char operation;
    unsigned short status;
    unsigned long  count;
  };
  
public:
 
  OptOutLoggingTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var): 
    BaseUnit(stat_var, task_name, params_var),
    conn_(open_pq())
  { }
 
  virtual ~OptOutLoggingTest() throw()
  { }
 
private:

  AutoTest::DBC::Conn conn_;
  AutoTest::ORM::StatsList<Stat> stats_;
  std::list<Diff> diffs_;
  AutoTest::Time time_;

  virtual bool run_test();

  void non_test_mode_();
  void test_mode_();
  void account_timezone_();
  
  template <size_t Count>
  void process_requests_(
    const std::string& description,
    AdClient& client,
    unsigned long colo,
    const Request (&requests) [Count],
    bool test = false);

  template <size_t Count>
  void initialize_stats_(
    const std::string& description,
    unsigned long colo,
    const Expected (&expects) [Count],
    bool test = false);
};

#endif // _UNITTEST__OPTOUTLOGGINGSTEST_
