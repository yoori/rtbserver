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

/// @file RoundedTimeIOTest.cpp
#include <iostream>
#include <LogCommons/LogCommons.ipp>

const Generics::Time TEST_TIME_DAY(
  String::SubString("2011-04-05"), "%Y-%m-%d");
const Generics::Time TEST_TIME_DAY_HOUR(
  String::SubString("2011-04-05_14:13:36"), "%Y-%m-%d_%H");
const Generics::Time TEST_TIME(
  String::SubString("2011-04-05_14:13:36"), "%Y-%m-%d_%H:%M:%S");

const char STANDARD_STREAM[] =
  "2011-04-05\t2011-04-05:14\t2011-04-05_14:13:36";

using namespace AdServer::LogProcessing;

class Tester
{
public:
  Tester(const Generics::Time& test_time)
    : times_(test_time)
  {
    ostr_ << times_.day << '\t' << times_.day_hour << '\t' << times_.seconds;
  }
  /**
   * @return If fail detected return true
   */
  bool
  check_fail()
  {
    TimeSet restored(ostr_);
    bool condition = times_.day == restored.day &&
      times_.day_hour == restored.day_hour &&
      restored.seconds == times_.seconds;
    if (!condition)
    {
      std::cerr << "Check fail:" << std::endl
        << "day=" << times_.day << ", restore=" << restored.day
        << std::endl
        << "day_hour=" << times_.day_hour << ", restore=" << restored.day_hour
        << std::endl
        << "seconds=" << times_.seconds << ", restore=" << restored.seconds
        << std::endl;
    }
    return !condition;
  }
  bool
  check_standard_fail()
  {
    if (ostr_.str() != STANDARD_STREAM)
    {
      std::cerr << "Output in stream failed, unexpected result="
        << ostr_.str() << std::endl;
      return true;
    }
    TimeSet restored(ostr_);
    bool condition =
      restored.seconds.time() == TEST_TIME &&
      restored.day.time() == TEST_TIME_DAY &&
      restored.day_hour.time() == TEST_TIME_DAY_HOUR &&
      restored.seconds == TEST_TIME &&
      restored.day == TEST_TIME_DAY &&
      restored.day_hour == TEST_TIME_DAY_HOUR;
    return !condition;
  }
private:
  struct TimeSet
  {
    TimeSet(Stream::Error& ostr)
    {
      Stream::Parser istr(ostr.str().data(), ostr.str().size());
      istr >> day >> day_hour >> seconds;
    }
    TimeSet(const Generics::Time& test_time)
      : day(test_time),
        day_hour(test_time),
        seconds(test_time)
    {}
    DayTimestamp day;
    DayHourTimestamp day_hour;
    SecondsTimestamp seconds;
  };

  TimeSet times_;
  Stream::Error ostr_;
};

int
main()
{
  try
  {
    std::cout << "RoundedTimeIOTest started.." << std::endl;
    int fails = 0;
    {
      Tester test(TEST_TIME);
      fails += test.check_fail();
      fails += test.check_standard_fail();
    }
    {
      Tester test(Generics::Time::get_time_of_day());
      fails += test.check_fail();
    }
    if (fails)
    {
      std::cerr << "Test FAILED, fails count=" << fails << std::endl;
    }
    else
    {
      std::cout << "SUCCESS" << std::endl;
    }
    return fails;
  }
  catch (const eh::Exception& ex)
  {
    std::cerr << "eh::Exception: " << ex.what() << std::endl;
  }
  catch (...)
  {
    std::cerr << "unknown exception" << std::endl;
  }
  return -1;
}
