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

#ifndef MATCHING_CHECK_TEST_HPP_
#define MATCHING_CHECK_TEST_HPP_

#include<tests/UnitTests/ChannelSvcs/Commons/ChannelServerTestCommons.hpp>

namespace AdServer
{
namespace UnitTests
{
  class MatchingCheckTest: public TestTemplate
  {
  public:
    int run() throw();
  private:
    typedef std::map<unsigned long, std::vector<std::string> > SaveType;

    void prepare_data_(SaveType& atoms_save)
      throw(eh::Exception);

    void make_check_(const SaveType& atoms)
      throw(eh::Exception);

    void match_i_(const String::SubString& in) throw(eh::Exception);

    void reset_staitstic_() throw(eh::Exception);
  };
}
}

#endif

