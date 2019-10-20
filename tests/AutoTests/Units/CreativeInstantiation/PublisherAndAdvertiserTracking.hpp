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


#ifndef _AUTOTEST__PUBLISHERANDADVERTISERTRACKING_
#define _AUTOTEST__PUBLISHERANDADVERTISERTRACKING_
  
#include <tests/AutoTests/Commons/Common.hpp>
  

class PublisherAndAdvertiserTracking : public BaseUnit
{

  typedef AutoTest::NSLookupRequest NSLookupRequest;
  typedef NSLookupRequest::NSLookupParam NSLookupParam;

public:

  struct TestRequest
  {
    const char* format;
    const char* tid;
    const char* colo;
    const char* testrequest;
    const char* ccid;
    const char* track_pixel;
    const char* cradvtrackpixel;
    const char* publ_tag_track_pixel;
  };
  
public:
  PublisherAndAdvertiserTracking(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var)
  {};

  virtual ~PublisherAndAdvertiserTracking() throw()
  {};

private:

  virtual bool run_test();

  // Utils
  void set_request_param(
    NSLookupParam& request_param,
    const char* param_name);
  
  void prepare_expected_body(
    std::ostringstream& body,
    const std::string& impression_url,
    const TestRequest& test);
};

#endif // _AUTOTEST__PUBLISHERANDADVERTISERTRACKING_
