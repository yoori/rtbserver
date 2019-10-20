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


#include "ChannelInventoryEstimLevelDown.hpp"
 
REFLECT_UNIT(ChannelInventoryEstimLevelDown) (
  "Statistics",
  AUTO_TEST_SLOW
);

namespace {
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::DBC::Conn DBConnection;
  typedef ORM::ChannelInventoryEstimStats::Diffs Diffs;
}
  
 
bool 
ChannelInventoryEstimLevelDown::run_test()
{
  DBConnection conn(open_pq());

  Generics::Time base_time = Generics::Time::get_time_of_day();

  if (base_time.get_gm_time().tm_hour >= 21)
  {
    base_time -= 3*60*60;
  }

  AdClient user(AdClient::create_user(this));

  NSLookupRequest request;
  request.referer_kw = fetch_string("KWS1");
  request.debug_time = base_time;

  {
   
    add_descr_phrase("After 1 hour and 1 munute");
    ORM::ChannelInventoryEstimStats stats;

    stats.key().
      channel_id(fetch_int("AdvChannelS1")).
      colo_id(1).
      match_level(0.5).
      sdate(base_time);

    stats.select(conn);

    std::string expected[] = {
      fetch_string("AdvBPPS1")
    };

    user.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        expected,
        user.debug_info.trigger_channels,
        AutoTest::SCE_ENTRY).check(),
      "trigger_channels");

    user.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        expected,
        user.debug_info.trigger_channels,
        AutoTest::SCE_ENTRY).check(),
      "trigger_channels");

    user.process_request(NSLookupRequest().
                         debug_time(base_time + 61*60));

    FAIL_CONTEXT(AutoTest::wait_checker(
        AutoTest::stats_diff_checker(
          conn,
          1,
          stats)).check(),
      "1 hour 1 munute check");
  }

  {
    add_descr_phrase("After 1 hour and 2 munutes");
    ORM::StatsArray<ORM::ChannelInventoryEstimStats, 2> stats;

    stats[0].key().
      channel_id(fetch_int("AdvChannelS1")).
      colo_id(1).
      match_level(0.5).
      sdate(base_time);
    stats[1].key().
      channel_id(fetch_int("AdvChannelS2")).
      colo_id(1).
      match_level(1.0).
      sdate(base_time);

    stats.select(conn);

    std::string expected[] = {
      fetch_string("AdvBPPS1"),
      fetch_string("AdvBPPS2")
    };
    
    request.referer_kw = fetch_string("KWS1") + "," +
        fetch_string("KWS2");
    request.debug_time = base_time + 62*60;
    
    user.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        expected,
        user.debug_info.trigger_channels,
        AutoTest::SCE_ENTRY).check(),
      "trigger_channels");

    const Diffs diff[2] =
    {
      Diffs(0),
      Diffs(1)
    };
    
    FAIL_CONTEXT(
      AutoTest::wait_checker(
        AutoTest::stats_diff_checker(
          conn, diff, stats)).check(),
      "1 hour 2 munutes check");
  }

  {
    add_descr_phrase("After 2 hour and 7 munutes");
    ORM::StatsArray<ORM::ChannelInventoryEstimStats, 2> stats;

    stats[0].key().
      channel_id(fetch_int("AdvChannelS1")).
      colo_id(1).
      match_level(0.5).
      sdate(base_time);
    stats[1].key().
      channel_id(fetch_int("AdvChannelS1")).
      colo_id(1).
      match_level(0.7).
      sdate(base_time);

    stats.select(conn);

    std::string expected[] = {
      fetch_string("AdvBPPS1")
    };


    request.referer_kw = fetch_string("KWS1");
    request.debug_time = base_time + 2*60*60 + 5*60;
    
    user.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        expected,
        user.debug_info.trigger_channels,
        AutoTest::SCE_ENTRY).check(),
      "trigger_channels");

    request.debug_time = base_time + 2*60*60 + 6*60;

    user.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        expected,
        user.debug_info.trigger_channels,
        AutoTest::SCE_ENTRY).check(),
      "trigger_channels");

    request.debug_time = base_time + 2*60*60 + 7*60;

    user.process_request(request);

    FAIL_CONTEXT(
      AutoTest::sequence_checker(
        expected,
        user.debug_info.trigger_channels,
        AutoTest::SCE_ENTRY).check(),
      "trigger_channels");

    const Diffs diff[2] =
    {
      Diffs(-1),
      Diffs(1)
    };


    FAIL_CONTEXT(
      AutoTest::wait_checker(
        AutoTest::stats_diff_checker(
          conn, diff, stats)).check(),
      "1 hour 2 munutes check");
  }

  return true;
}

