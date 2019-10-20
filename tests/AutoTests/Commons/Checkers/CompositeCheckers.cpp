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


#include "CompositeCheckers.hpp"
#include <tests/AutoTests/Commons/BaseUnit.hpp>

namespace AutoTest
{
  namespace Internals
  {
    // Internals::CheckerHolder
    
    CheckerHolder::~CheckerHolder() throw ()
    { }

    // Internals::SubCheckersHolder
  
    SubCheckersHolder::~SubCheckersHolder()
      throw()
    { }

    int get_unit_timeout()
    {
      return BaseUnit::timeout();
    }
  }

  // OrChecker
  
  OrChecker::~OrChecker() throw()
  { }

  bool
  OrChecker::check(bool throw_error)
    throw (CheckFailed, eh::Exception)
  {
    std::list<CheckFailed> errors;
    bool result = false;
    size_t index = 0;

    for(CheckerHolderList::iterator checker_it = sub_checkers_.begin();
        checker_it != sub_checkers_.end(); ++checker_it, ++index)
    {
      try
      {
        if((*checker_it)->check(throw_error))
        {
          if (counter_)
          {
            counter_->incr(index);
          }
          result = true;
        }
      }
      catch(const CheckFailed& ex)
      {
        errors.push_back(ex);
      }
    }

    if(throw_error && !result)
    {
      Stream::Error ostr;
      ostr << "all checks failed:";
      for(std::list<CheckFailed>::const_iterator ex_it =
            errors.begin();
          ex_it != errors.end(); ++ex_it)
      {
        ostr << std::endl << "  " << ex_it->what();
      }

      throw CheckFailed(ostr);
    }

    return result;
  }


  // CountChecker
  
  CountChecker::CountChecker(
    size_t events_size,
    size_t sample_size) :
    counts_(events_size, 0),
    sample_size_(sample_size)
  { }

  CountChecker::~CountChecker() throw()
  { }

  void
  CountChecker::incr(size_t index)
  {
    if (index >= counts_.size())
    {
      return;
    }
    counts_[index]++;
  }

  bool
  CountChecker::check(bool throw_error)
    throw (AutoTest::CheckFailed, eh::Exception)
  {
    size_t i = 0;
    for (auto it = counts_.begin(); it != counts_.end(); it++, ++i)
    {
      if (!*it )
      {
        if (throw_error)
        {
          Stream::Error ostr;
          ostr << "The condition#"  + strof(i+1) + " is not satisfied";
          throw AutoTest::CheckFailed(ostr);
        }
        return false;
      }
    }
    return true;
  }

  const std::vector<unsigned long>&
  CountChecker::counts() const
  {
    return counts_;
  }

  // AndChecker

  AndChecker::~AndChecker() throw()
  { }

  bool
  AndChecker::check(bool throw_error)
    throw (CheckFailed, eh::Exception)
  {
    // throw first appeared exception with keeping it type
    for(CheckerHolderList::iterator checker_it = sub_checkers_.begin();
        checker_it != sub_checkers_.end(); ++checker_it)
    {
      if(!(*checker_it)->check(throw_error))
      {
        return false;
      }
    }

    return true;
  }

  

}

