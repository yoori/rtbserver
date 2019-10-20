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


#ifndef _UNITTEST__TAGSTEST_
#define _UNITTEST__TAGSTEST_

#include <tests/AutoTests/Commons/Common.hpp>

typedef AutoTest::NSLookupRequest  NSLookupRequest;
typedef AutoTest::AdClient AdClient;

/**
* @class TagsTest
* @brief Test for checking tag pricing logic
*/

class TagsTest: 
  public BaseUnit
{
public:
 
  TagsTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var): 
    BaseUnit(stat_var, task_name, params_var)  {};
 
  virtual ~TagsTest() throw()
  {};

private:
 
  virtual bool run_test();

  void set_up();

  // test cases
  void empty_country_case();
  void country_ru_case();
  void country_us_case();
  void unknown_country_case();
  void cpm_filter_case();
  
  NSLookupRequest request;
  std::string tag_id1;
  std::string tag_id2;
  std::string tag_id3;
  
  std::string siterate_id1;
  std::string siterate_id2;
  std::string siterate_id5;
  std::string siterate_id_def;

  std::string cc_id1;
  std::string cc_id2;
  std::string cc_id3;
  std::string cc_id4;
  std::string cc_id5;
  std::string cc_id6;
 
};

#endif  // _UNITTEST__TAGSTEST_
