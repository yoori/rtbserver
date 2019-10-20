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

#ifndef AUTOTESTS_COMMONS_REFLECTION_HPP
#define AUTOTESTS_COMMONS_REFLECTION_HPP

#include <tests/AutoTests/Commons/BaseUnit.hpp>


typedef BaseUnit* (BaseUnitConstructor) (UnitStat&   stat_var,
                                         const char* test_name,
                                         XsdParams   params_var);
enum AutoTestSpeedGroup
{
  AUTO_TEST_FAST  = 1,
  AUTO_TEST_SLOW  = 2,
  AUTO_TEST_QUIET = 4
};

enum AutoTestSerialize
{
  AUTO_TEST_RANDOM = 0,
  AUTO_TEST_SERIALIZE = 1,
  AUTO_TEST_SERIALIZE_PRE = 2
};

struct UnitDescriptor;
extern UnitDescriptor* base_unit_descriptor;

struct UnitDescriptor
{
  std::string category;
  const int group;
  AutoTestSerialize serialize;
  // auto part
  std::string name;
  BaseUnitConstructor* constructor;
  UnitDescriptor* next;
  bool dirty;
  
  UnitDescriptor (const char* c,
                  const int g,
                  AutoTestSerialize s,
                  const char* n,
                  BaseUnitConstructor* co);
};

struct UnitDescriptorD
{
  const char* name;
  BaseUnitConstructor* constructor;
};

template<UnitDescriptorD& d>
struct UnitDescriptorT :
  public UnitDescriptor
{
  UnitDescriptorT (const char* c,
                   const int g,
                   AutoTestSerialize s = AUTO_TEST_RANDOM)
    : UnitDescriptor(c, g, s, d.name, d.constructor)
  {
  }
};

extern
std::ostream&
operator<< (std::ostream& strm, const UnitDescriptor& descriptor);

#define CONSTRUCT_UNIT(name) AT_INTERNAL_construct_##name

#define CONSTRUCTOROF_UNIT(name)                                        \
  extern "C" BaseUnit* CONSTRUCT_UNIT(name) (UnitStat&   stat_var,      \
                                             const char* test_name,     \
                                             XsdParams   params_var)    \
  {                                                                     \
    return new name (stat_var, test_name, params_var);                  \
  }

#define REFLECT_UNIT(name)                                              \
  CONSTRUCTOROF_UNIT(name);                                             \
  UnitDescriptorD AT_INTERNAL_reflect_D_##name = {                      \
    "" # name,                                                          \
    CONSTRUCT_UNIT(name) };                                             \
  UnitDescriptorT< AT_INTERNAL_reflect_D_##name > AT_INTERNAL_reflect_##name

#endif//AUTOTESTS_COMMONS_REFLECTION_HPP
