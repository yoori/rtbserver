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


#include "RemoteColoUpdateStats.hpp"

namespace
{
  typedef AutoTest::AdClient AdClient;
  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef AutoTest::ChannelsCheck ChannelsCheck;
  typedef AutoTest::SelectedCreativeChecker CreativeChecker;

  class ColoStatsChecker : public AutoTest::Checker
  {
    typedef AutoTest::ORM::PQ::Colostats ColoStats;
    typedef AutoTest::ORM::ORMDate Date;
    typedef AutoTest::ORM::ORMString String;
    
  public:
    ColoStatsChecker(
      AutoTest::DBC::IConn& conn,
      unsigned long colo,
      const Generics::ExtendedTime& date,
      const std::string& version) :
      conn_(conn),
      colo_(colo),
      date_(date),
      version_(version)
    { }
    
    virtual ~ColoStatsChecker() throw()
    { }

    bool
    check(
      bool throw_error = true)
      throw(eh::Exception)
    {
      ColoStats stats(conn_, colo_);

      if ( stats.select() &&
        stats.software_version == version_ &&
        stats.last_campaign_update == date_ &&
        stats.last_channel_update == date_ &&
        stats.last_stats_upload == date_ )
      {
        return true;
      }
      
      if (throw_error)
      {
        Stream::Error error;
        error << "Expect" <<
          "{" << std::endl <<
          "  version = " << strof(version_) << std::endl <<
          "  update = " << strof(date_) << std::endl <<
          "}" << std::endl <<
          "Got:" << std::endl <<
          "{" << std::endl <<
          "  version = " << strof(stats.software_version) << std::endl <<
          "  last_campaign_update = " << strof(stats.last_campaign_update) << std::endl <<
          "  last_channel_update = " << strof(stats.last_channel_update) << std::endl <<
          "  last_stats_upload = " << strof(stats.last_stats_upload) << std::endl << "}";
        throw AutoTest::CheckFailed(error);
      }
      return false;
    }
  private:
    AutoTest::DBC::IConn& conn_;
    unsigned long colo_;
    Date date_;
    String version_;
  };
}

REFLECT_UNIT(RemoteColoUpdateStats) (
  "Statistics",
  AUTO_TEST_SLOW,
  AUTO_TEST_SERIALIZE_PRE);

bool
RemoteColoUpdateStats::run_test()
{
  AdClient client(AdClient::create_user(this));

  AutoTest::DBC::Conn conn(open_pq());

  FAIL_CONTEXT(
    AutoTest::predicate_checker(
      get_config().check_service(
        CTE_REMOTE2, STE_FRONTEND)),
    "Remote configuration required");

  AutoTest::Time debug_time;
  
  for (int i = 0; i < 2; ++i)
  {
    client.process_request(
      NSLookupRequest().
        referer_kw(fetch_string("KWD")).
        debug_time(debug_time));

    FAIL_CONTEXT(
      ChannelsCheck(
        this,
        "CHANNEL",
        client.debug_info.history_channels).check(),
      "Expected history_channels");
    
    FAIL_CONTEXT(
      CreativeChecker(
        client,
        NSLookupRequest().
          tid(fetch_int("TAG")).
          debug_time(debug_time),
        fetch_int("CC")).check(),
      "Expected ccid");

    client.change_base_url(
      get_config().get_service(
        CTE_REMOTE2, STE_FRONTEND).address.c_str());
  }

  const unsigned long COLOCATIONS[] =
  {
    fetch_int("CENTRAL/COLO"),
    fetch_int("REMOTE/COLO/1"),
    fetch_int("REMOTE/COLO/2")
  };

  for (size_t i = 0; i < countof(COLOCATIONS); ++i)
  {
    FAIL_CONTEXT(
      AutoTest::wait_checker(
        ColoStatsChecker(
          conn,
          COLOCATIONS[i],
          debug_time.get_gm_time().get_date(),
          fetch_string("VERSION"))).check(),
      " Colo#" + strof(i+1));
  }
      
  return true;
}

