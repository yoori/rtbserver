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

#ifndef _AUTOTEST__SITEUSERSTATSTEST_
#define _AUTOTEST__SITEUSERSTATSTEST_
  
#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = AutoTest::ORM;
namespace DBC = AutoTest::DBC;

class SiteUserStatsTest : public BaseUnit
{

  typedef AutoTest::AdClient AdClient;
  
  struct Expected
  {
    const char* site;
    const char* colo;
    AutoTest::Time last_appearance;
    AutoTest::Time isp_sdate;
    int count;
  };

  struct Request
  {
    const char* tag;
    AutoTest::Time time;
  };
  
  typedef ORM::SiteUserStats SiteUserStat;
  typedef ORM::StatsList<SiteUserStat> SiteUserStats;
  typedef SiteUserStat::Diffs Diff;
  typedef std::list<Diff> Diffs;

public:
  SiteUserStatsTest(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var),
    conn_(open_pq())
  { }

  virtual ~SiteUserStatsTest() throw()
  { }

private:

  AutoTest::Time base_time_;

  // utils
  template <size_t Count>
  void add_stats_(
    const std::string& description,
    const Expected (&expected)[Count]);

  template <size_t Count>
  void process_requests_(
    AdClient& client,
    const Request (&expected)[Count],
    const char* colo = 0);

  void
  check_stats_();

  // cases
  void
  unique_users_stats_();
  
  void
  last_appearance_date_();
  
  void
  non_gmt_timezone_();
  
  void
  colo_logging_();
  
  AdClient
  async_part_1_();
  
  void
  async_part_2_(
    AdClient& client);
  
  void
  async_part_3_(
    AdClient& client);
  
  void
  temporary_user_();
  
  void
  non_opted_in_users_();
  
  virtual bool run_test();

  SiteUserStats stats_;
  SiteUserStats sum_stats_;
  Diffs diffs_;
  Diffs sum_diffs_;
  DBC::Conn conn_;
};

#endif // _AUTOTEST__SITEUSERSTATSTEST_
