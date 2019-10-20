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

#include "UserProfilesExchangeCommon.hpp"

namespace AutoTest
{
  namespace UserProfilesExchange
  {

    // Class CheckWaitHistoryChannel
    
    CheckWaitHistoryChannel::CheckWaitHistoryChannel (
        AutoTest::AdClient& client,
        unsigned long channel,
        const char* colo,
        unsigned long wait_time,
        const NSLookupRequest& request)
      : client_(client),
        colo_(colo),
        wait_time_(wait_time),
        request_(request.url())
    {
      expected_.push_back(strof(channel));
    }

    CheckWaitHistoryChannel::CheckWaitHistoryChannel (
        AutoTest::AdClient& client,
        unsigned long channels[],
        size_t channel_size,
        const char* colo,
        unsigned long wait_time,
        const NSLookupRequest& request)
      : client_(client),
        colo_(colo),
        wait_time_(wait_time),
        request_(request.url())
    {
      for (size_t i = 0; i < channel_size; ++i)
      {
        expected_.push_back(strof(channels[i]));
      }
    }

    CheckWaitHistoryChannel::~CheckWaitHistoryChannel() throw()
    {}

    bool
    CheckWaitHistoryChannel::check(bool throw_error)
        throw(eh::Exception)
    {
      for(unsigned int my_time = 0; my_time < wait_time_; my_time += SLEEP_TIME)
      {
        client_.process_request(request_.c_str());

        FAIL_CONTEXT(
          AutoTest::equal_checker(
            colo_,
            client_.debug_info.colo_id).check(),
          "must receive expected colo");
        
        if (AutoTest::entry_in_seq(expected_,
              client_.debug_info.history_channels))
        {
          return true;
        }
        AutoTest::Shutdown::instance().wait(SLEEP_TIME);
      }
      if(throw_error)
      {
        Stream::Error ostr;
        ostr << "Can't get history channels " <<
            AutoTest::seq_to_str(expected_);
        throw AutoTest::CheckFailed(ostr);
      }
      return false;
    }

  }
}

