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


#ifndef _AUTOTEST__URLENCODINGTEST_
#define _AUTOTEST__URLENCODINGTEST_
  
#include <tests/AutoTests/Commons/Common.hpp>
  
typedef AutoTest::NSLookupRequest NSLookupRequest;

class URLEncodingTest : public BaseUnit
{
public:
  URLEncodingTest(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var)
  {};

  virtual ~URLEncodingTest() throw()
  {};

private:

  void
  passback_encoding(
    const char* tag,
    const char* passback,
    NSLookupRequest::Member param,
    bool with_frontend,
    const char* expected_passback);

  void
  tag_passback_encoding(
    const char* tag,
    const char* expected_passback);
  
  void
  clickurl_encoding();
  void
  clickurl_relocate_encoding();
  void
  optout_redirect();

  void
  clickurl_preclick_encoding(
    const char* preclick,
    const char* expected_redirect);

  virtual bool run_test();
};

#endif // _AUTOTEST__URLENCODINGTEST_
