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

#include <tests/AutoTests/Commons/Common.hpp>


namespace AutoTest
{

  // Utils

  long
  find_local_params(
    const LocalConfig& local,
    const char *name)
  {
    unsigned long units_count = local.UnitLocalData().size();
    for (unsigned long loc_idx = 0; loc_idx < units_count; ++loc_idx)
    {
      if (strcmp(local.UnitLocalData()[loc_idx].UnitName().c_str(),
                 name) == 0)
        return loc_idx;
    }
    return -1;
  }

  long find_global_parameter(const
    LocalUnit& global_section,
    const char *name)
  {
    size_t locals_len = global_section.DataElem().size();

    for (size_t ind = 0; ind < locals_len; ++ind)
    {
      if (global_section.DataElem()[ind].Name() == name)
        return ind;
    }
    return -1;
  }

  //Constants

  const char* GlobalSettings_::GLOBAL_SECTION_NAME = "Global";
  const char* GlobalSettings_::OPTOUT_COLO_NAME = "OptoutColo";

  // class GlobalSettings_

  GlobalSettings_::GlobalSettings_() :
    wait_timeout_(DEFAULT_TIMEOUT),
    frontend_timeout_(DEFAULT_TIMEOUT),
    optout_colo_(DEFAULT_OPTOUT_COLO),
    config_(0)
  { }

  void
  GlobalSettings_::initialize(
    const GlobalConfig& global,
    const LocalConfig& local)
  {

    config_ = &global;

    wait_timeout_ =
        global.get_params().TimeOuts().wait_timeout();

    frontend_timeout_ =
      global.get_params().TimeOuts().frontend_timeout();


    int global_idx =
      find_local_params(
        local,
        GLOBAL_SECTION_NAME);

    if (global_idx >= 0)
    {

      Locals global_section =
        local.UnitLocalData()[global_idx];

      int optout_colo_idx =
        find_global_parameter(
          global_section,
          OPTOUT_COLO_NAME);

      if (optout_colo_idx >= 0)
      {
        Stream::Parser istr(
          global_section.
          DataElem()[optout_colo_idx].Value());
        istr >> optout_colo_;
      }
    }
  }
}
