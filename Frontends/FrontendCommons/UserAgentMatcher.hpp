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

#ifndef _FRONTENDCOMMONS_USERAGENTMATCHER_HPP_
#define _FRONTENDCOMMONS_USERAGENTMATCHER_HPP_

#include <string>
#include <eh/Exception.hpp>
#include <Generics/GnuHashTable.hpp>
#include <Generics/HashTableAdapters.hpp>

namespace FrontendCommons
{
  class UserAgentMatcher
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    UserAgentMatcher(): hash_set_() {}

    void init(const char *filename) throw(Exception, eh::Exception);

    bool match(const char *user_agent) const
    {
      return hash_set_.find(user_agent) == hash_set_.end() ? false : true;
    }

  private:
    typedef Generics::GnuHashSet<Generics::StringHashAdapter> HashSetT;

    HashSetT hash_set_;
  };
} // namespace FrontendCommons

#endif /* _FRONTENDCOMMONS_USERAGENTMATCHER_HPP_ */

