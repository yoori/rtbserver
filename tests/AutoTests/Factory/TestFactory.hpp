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

#ifndef __TESTFACTORY_HPP
#define __TESTFACTORY_HPP

#include <tests/AutoTests/Commons/Common.hpp>

namespace TestFactory
{
  typedef std::set<std::string> StringList;
  typedef std::set<AutoTestSpeedGroup> GroupList;
  typedef std::list<UnitDescriptor*> UnitsList;
  typedef std::vector<UnitDescriptor*> UnitsSeq;
  typedef const xsd::tests::AutoTests::UnitLocalDataType& Locals;

  DECLARE_EXCEPTION(Exception, eh::Exception);
  DECLARE_EXCEPTION(InvalidArgument, Exception);

  class TestFactory
  {
    
  public:
    TestFactory() throw();
    
    ~TestFactory() throw();

    void
    filter(
      const StringList& exclude_tests,
      const StringList& exclude_categories,
      const StringList& tests,
      const GroupList& groups,
      const StringList& categories,
      int select_serialized)
      throw ();

    void
    filter(
      const StringList& tests)
      throw ();

    const UnitsList&
    units()
      throw ();

  private:
    UnitsList units_;
  };

}; //namespace
  

#endif  // __TESTFACTORY_HPP
