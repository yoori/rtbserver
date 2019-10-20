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

#ifndef _AUTOTEST__SEARCHENGINEDYNAMICUPDATETEST_
#define _AUTOTEST__SEARCHENGINEDYNAMICUPDATETEST_
 
#include <tests/AutoTests/Commons/Common.hpp>

namespace ORM = ::AutoTest::ORM;

 
class SearchEngineDynamicUpdateTest: public BaseDBUnit
{
  typedef AutoTest::SearchEngineChecker::Expected Expected;
  typedef Expected& (Expected::*ExpectedSetter)(const std::string&);
  
public:

  SearchEngineDynamicUpdateTest(
      UnitStat& stat_var, 
      const char* task_name, 
      XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var)
  {};
 
  virtual ~SearchEngineDynamicUpdateTest() throw()
  {};
 
private:


  void add_scenario();

  template<typename AdminField, typename ValueType>
  void
  update_scenario(
    const std::string& description,
    unsigned long engine_id,
    AdminField admin_field,
    ExpectedSetter diff_setter,
    const ValueType& old_value,
    const ValueType& new_value);
  
  void delete_scenario();
  
  bool run();
  
  void set_up();

  void tear_down();
 
};

#endif //_AUTOTEST__SEARCHENGINEDYNAMICUPDATETEST_

