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


#ifndef _UNITTEST__NOADVNOTRACKTEST_
#define _UNITTEST__NOADVNOTRACKTEST_

#include <tests/AutoTests/Commons/Common.hpp>

class NoAdvNoTrackTest: public BaseUnit
{
  typedef AutoTest::AdClient AdClient;

public:

  NoAdvNoTrackTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var);
 
  virtual ~NoAdvNoTrackTest() throw();
 
private:
  std::string no_track_words;
  std::string no_track_url_words;
  std::string no_track_urls;
  std::string no_adv_words;
  std::string no_adv_url_words;
  std::string no_adv_urls;
  std::string uid;

  virtual bool run_test();

  // Test cases
  void
  no_adv_case_(
    AdClient& client);

  void
  no_track_case_(
    AdClient& client);

  void
  no_adv_and_no_track_case_(
    AdClient& client);

  void
  session_info_case_();

  void
  passback_case_();
};

#endif  // _UNITTEST__NOADVNOTRACKTEST_

