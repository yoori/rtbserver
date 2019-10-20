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


#ifndef _AUTOTEST__TAGSANDCREATIVESSIZEMATCHING_
#define _AUTOTEST__TAGSANDCREATIVESSIZEMATCHING_
  
#include <tests/AutoTests/Commons/Common.hpp>

class TagsAndCreativesSizeMatching : public BaseUnit
{
public:
  TagsAndCreativesSizeMatching(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var) :
    BaseUnit(stat_var, task_name, params_var)
  { }

  virtual ~TagsAndCreativesSizeMatching() throw()
  { }

private:

  virtual bool run_test();

  // Cases
  void same_type_diff_sizes();
  void actual_size();
  void diff_sizes();
  void max_ads();
  void zero_max_ads();
  void multi_creatives_ccg();
  void display_vs_text();
  void size_type_level();
  void rtb();
};

#endif // _AUTOTEST__TAGSANDCREATIVESSIZEMATCHING_
