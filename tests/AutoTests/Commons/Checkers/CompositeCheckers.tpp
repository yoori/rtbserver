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


namespace AutoTest
{
  namespace Internals
  {
    //  Internals::CheckerHolderImpl

    template<typename CheckerType>
    CheckerHolderImpl<CheckerType>::~CheckerHolderImpl() throw ()
    { }

    template<typename CheckerType>
    CheckerHolderImpl<CheckerType>::
    CheckerHolderImpl(const CheckerType& init)
      throw()
      : checker_(init)
    {}

    template<typename CheckerType>
    CheckerHolder_var
    CheckerHolderImpl<CheckerType>::
    clone() const throw()
    {
      return new CheckerHolderImpl<CheckerType>(*this);
    }

    template<typename CheckerType>
    bool
    CheckerHolderImpl<CheckerType>::
    check(bool throw_error) throw(CheckFailed, eh::Exception)
    {
      return checker_.check(throw_error);
    }

    // Internals::SubCheckersHolder

    inline
    SubCheckersHolder::SubCheckersHolder() throw()
    { }
    
    template<typename SubCheckerType, typename... SubCheckers>
    void
    SubCheckersHolder::add_sub_checker(
      const SubCheckerType& sub_checker,
      SubCheckers... sub_checkers)
      throw()
    {
      add_sub_checker(sub_checker);
      add_sub_checker(sub_checkers...);
    }

    inline
    SubCheckersHolder::SubCheckersHolder(const SubCheckersHolder& init)
      throw()
    {
      for(CheckerHolderList::const_iterator checker_it =
            init.sub_checkers_.begin();
          checker_it != init.sub_checkers_.end(); ++checker_it)
      {
        sub_checkers_.push_back((*checker_it)->clone());
      }
    }

    template<typename SubCheckerType>
    void
    SubCheckersHolder::add_sub_checker(const SubCheckerType& sub_checker)
      throw()
    {
      sub_checkers_.push_back(new CheckerHolderImpl<SubCheckerType>(sub_checker));
    }

  }

  // WaitChecker
  
  template<typename SubCheckerType>
  WaitChecker<SubCheckerType>::~WaitChecker() throw()
  { }

  template<typename SubCheckerType>
  WaitChecker<SubCheckerType>::WaitChecker(
    const SubCheckerType& sub_checker,
    unsigned long wait_time,
    unsigned long sleep_time)
    : sub_checker_(sub_checker),
      wait_time_(wait_time),
      sleep_time_(sleep_time)
  {}

  template<typename SubCheckerType>
  bool
  WaitChecker<SubCheckerType>::check(bool throw_error)
    throw (CheckFailed, eh::Exception)
  {
    Generics::Time start_loop_time = Generics::Time::get_time_of_day();
    Generics::Time now = start_loop_time;
    int timeout = wait_time_? wait_time_: AutoTest::Internals::get_unit_timeout();

    while(now < start_loop_time + timeout)
    {
      if(sub_checker_.check(false))
      {
        return true;
      }
      Shutdown::instance().wait(sleep_time_);
      now = Generics::Time::get_time_of_day();
    }

    return sub_checker_.check(throw_error);
  }

  // NotChecker
  
  template<typename SubCheckerType>
  NotChecker<SubCheckerType>::~NotChecker() throw()
  { }
  
  template<typename SubCheckerType>
  NotChecker<SubCheckerType>::NotChecker(const SubCheckerType& sub_checker)
    : sub_checker_(sub_checker)
  {}

  template<typename SubCheckerType>
  bool
  NotChecker<SubCheckerType>::check(bool throw_error)
    throw (CheckFailed, eh::Exception)
  {
    bool result = true;
    try
    {
      result = sub_checker_.check(throw_error);
    }
    catch (const eh::Exception& e)
    {
      return true;
    }

    if (result && throw_error)
    {
      Stream::Error error;
      error << "Not checker got true value!" << std::endl;
      throw CheckFailed(error);
    }

    return !result;
  }

  // ThrowChecker
  
  template<typename SubCheckerType>
  ThrowChecker<SubCheckerType>::~ThrowChecker() throw()
  { }
  
  template<typename SubCheckerType>
  ThrowChecker<SubCheckerType>::ThrowChecker(
    const SubCheckerType& sub_checker, bool throw_error)
    : sub_checker_(sub_checker),
      throw_error_(throw_error)
  {}

  template<typename SubCheckerType>
  bool
  ThrowChecker<SubCheckerType>::check(bool /*throw_error*/)
    throw (CheckFailed, eh::Exception)
  {
    return sub_checker_.check(throw_error_);
  }

  // FailChecker

  template<typename MasterChecker, typename SlaveChecker>
  FailChecker<MasterChecker,SlaveChecker>::FailChecker(
    const MasterChecker& master,
      const SlaveChecker& slave) :
    master_(master),
    slave_(slave)
  { }
  
  template<typename MasterChecker, typename SlaveChecker>
  FailChecker<MasterChecker,SlaveChecker>::~FailChecker() throw()
  { }
  
  template<typename MasterChecker, typename SlaveChecker>
  bool
  FailChecker<MasterChecker,SlaveChecker>::check(bool throw_error)
    throw (CheckFailed, eh::Exception)
  {
    try
    {
      return master_.check(throw_error);
    }
    catch (const eh::Exception&)
    {
      slave_.check(throw_error);
      throw;
    }
  }
  

  // OrChecker

  inline
  OrChecker::OrChecker(
    ICounter* counter) throw():
    counter_(counter) 
  { }
  
  template<typename... SubCheckers>
  OrChecker::OrChecker(
    SubCheckers... sub_checkers) :
    counter_(0)
  {
    add_sub_checker(sub_checkers...);
  }

  template<typename... SubCheckers>
  OrChecker&
  OrChecker::or_if(
    SubCheckers... sub_checkers)
  {
    add_sub_checker(sub_checkers...);
    return *this;
  }

  // AndChecker
  
  template<typename... SubCheckers>
  AndChecker::AndChecker(
    SubCheckers... sub_checkers)
    throw()
  {
    add_sub_checker(sub_checkers...);
  }

  template<typename... SubCheckers>
  AndChecker&
  AndChecker::and_if(SubCheckers... sub_checkers)
    throw()
  {
    add_sub_checker(sub_checkers...);
    return *this;
  }

  // helpers implementation
  
  template<typename SubCheckerType>
  WaitChecker<SubCheckerType>
  wait_checker(
    const SubCheckerType& sub_checker,
    unsigned long wait_time,
    unsigned long sleep_time)
  {
    return WaitChecker<SubCheckerType>(
      sub_checker,
      wait_time,
      sleep_time);
  }

  template<typename SubCheckerType>
  ThrowChecker<SubCheckerType>
  throw_checker(
    const SubCheckerType& sub_checker,
    bool throw_error)
  {
    return ThrowChecker<SubCheckerType>(
      sub_checker,
      throw_error);
  }

  template<typename MasterChecker, typename SlaveChecker>
  FailChecker<MasterChecker, SlaveChecker>
  fail_checker(
    const MasterChecker& master,
    const SlaveChecker& slave)
  {
    return
      FailChecker<MasterChecker, SlaveChecker>(
        master,
        slave);
  }

  template<typename... SubCheckers>
  OrChecker
  or_checker(
    SubCheckers... sub_checkers)
  {
    return OrChecker(sub_checkers...);
  }

  template<typename... SubCheckers>
  OrChecker
  or_count_checker(
    OrChecker::ICounter* counter,
    SubCheckers... sub_checkers)
  {
    return OrChecker(counter).or_if(sub_checkers...);
  }

  template<typename... SubCheckers>
  AndChecker
  and_checker(
    SubCheckers... sub_checkers)
  {
    return AndChecker(sub_checkers...);
  }

  template<typename SubCheckerType>
  NotChecker<SubCheckerType>
  not_checker(
    const SubCheckerType& checker)
  {
    return   NotChecker<SubCheckerType>(checker);
  }

}

