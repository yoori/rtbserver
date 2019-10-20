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


#include "SessionExpiration.hpp"

namespace
{
  typedef AutoTest::NSLookupRequest  NSLookupRequest;
  typedef AutoTest::AdClient AdClient;

  /**
   * @class SessionMatchesCheck
   * @brief Check session_matches field in base profile (by UserInfoAdmin)
   */
  class SessionMatchesCheck : public AutoTest::Checker
  {
  public:

    typedef std::list<AutoTest::Time> TimeStamps;
    typedef AutoTest::BaseProfileChecker BaseProfileChecker;
    
  public:
    /**
     * @brief Constructor.
     *
     * @param test.
     * @param client.
     * @param channel name.
     * @param expected timestamps.
     * @param check description.
     */
    SessionMatchesCheck(
      BaseUnit* test,
      AdClient& client,
      const char* channel,
      const TimeStamps& timestamps,
      const std::string& description) :
      test_(test),
      client_(client),
      channel_(channel),
      timestamps_(timestamps),
      description_(description)
    { }
      
    /**
     * @brief Destructor.
     */
    virtual ~SessionMatchesCheck() throw()
    { }
    
    /**
     * @brief Check.
     * @param throw on error flag.
     */
    bool check(bool throw_error = true)
      throw(eh::Exception)
    {
      std::ostringstream expected_session;
      expected_session << "\\[ channel_id = " <<
        test_->fetch_string(channel_) << ", timestamps = ";

      TimeStamps::const_iterator it = timestamps_.begin();
      while (it != timestamps_.end())
      {
        expected_session << it->get_gm_time().format("%Y-%m-%d %H:%M:%S");
        if (++it != timestamps_.end())
        {
          expected_session <<  ", ";
        }
      }

      expected_session << " \\]";

      return
        BaseProfileChecker(
          test_,
          AutoTest::prepare_uid(
            client_.get_uid(),
            AutoTest::UUE_ADMIN_PARAMVALUE),
          false,
          AutoTest::UserInfoManagerController,
          BaseProfileChecker::Expected().
            url_session_matches(expected_session.str())).check(throw_error);
    }

  private:
    BaseUnit* test_;
    AdClient& client_;
    std::string channel_;
    const TimeStamps& timestamps_;
    std::string description_;
  };
}

REFLECT_UNIT(SessionExpiration) (
  "UserProfiling",
  AUTO_TEST_FAST);

bool
SessionExpiration::run_test()
{
  AdClient client(AdClient::create_user(this));
  AutoTest::Time time;
  SessionMatchesCheck::TimeStamps timestamps;

  timestamps.push_back(time);

  NSLookupRequest request;
  request.referer = fetch_string("Ref");
  request.debug_time(time);

  client.process_request(request);

  FAIL_CONTEXT(
    AutoTest::entry_checker(
      fetch_string("BP"),
      client.debug_info.trigger_channels).check(),
    "Trigger channels check");

  FAIL_CONTEXT(
    SessionMatchesCheck(
      this, client, "Channel",
      timestamps, "After 1st request.").check());

  // 5 minutes later
  time+=5*60;
  timestamps.push_back(time);
  request.debug_time(time);
  client.process_request(request);

  FAIL_CONTEXT(
    SessionMatchesCheck(
      this, client, "Channel",
      timestamps, "After 2nd request.").check());

  // 10 minutes later
  // 1st timestamp should be removed from url_session_matches
  time+=5*60 + 1;
  timestamps.pop_front();  
  timestamps.push_back(time);
  request.debug_time(time);
  client.process_request(request);

  FAIL_CONTEXT(
    SessionMatchesCheck(
      this, client, "Channel",
      timestamps, "After 3d request.").check());

  // 15 minutes later
  // 2nd timestamp should be removed from url_session_matches
  time+=5*60;
  timestamps.pop_front();
  client.process_request(
    NSLookupRequest().debug_time(time));

  FAIL_CONTEXT(
    SessionMatchesCheck(
      this, client, "Channel",
      timestamps, "After 4th request.").check());
 
  return true;
}

