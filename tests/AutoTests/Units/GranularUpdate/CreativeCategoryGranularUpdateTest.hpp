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


#ifndef _UNITTEST__CREATIVECATEGORYGRANULARUPDATETEST_
#define _UNITTEST__CREATIVECATEGORYGRANULARUPDATETEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = ::AutoTest::ORM;
 
/**
* @class CreativeCategoryGranularUpdateTest
* @brief Test creative category granular update.
*        Attention for special rules for creative
*        categories with cct_id=2 (Tags)
*/
 
class CreativeCategoryGranularUpdateTest: public BaseDBUnit
{
public:
 
  CreativeCategoryGranularUpdateTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~CreativeCategoryGranularUpdateTest() throw()
  {};

protected:
  void set_up();
  void tear_down();
  bool run();

private:
  void create_categories_();
  void unlink_category_();
  void add_tags_exclusion_();
  void del_tags_exclusion_();
};

#endif  // _UNITTEST__CREATIVECATEGORYGRANULARUPDATETEST_
