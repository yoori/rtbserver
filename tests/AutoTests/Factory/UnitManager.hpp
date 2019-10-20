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

#ifndef _UNITTEST__UNITMANAGER_
#define _UNITTEST__UNITMANAGER_

#include <eh/Exception.hpp>
#include <Generics/TaskRunner.hpp>
#include <Sync/SyncPolicy.hpp>

#include <XMLUtility/Utility.hpp>

#include <tests/AutoTests/Commons/Common.hpp>

#include <vector>

#include "TestFactory.hpp"

/**
 * @class UnitManager
 *
 * @brief Root class for running test units.
 *
 * Loads configuration, creates infrastructure and runs test units.
 */

class UnitManager
{
  class TaskTest : public virtual Generics::TaskImpl
  {
    typedef std::unique_ptr<BaseUnit> BaseUnit_var;
  public:
    
    TaskTest(
      UnitManager* manager,
      UnitDescriptor* descriptor,
      const GlobalConfig& config,
      AllLocals locals);

    virtual
    void
    execute() throw (eh::Exception);

  protected:

    virtual
    ~TaskTest() throw ();

  private:
    UnitManager* manager_;
    UnitDescriptor* descriptor_;
    const GlobalConfig& config_;
    AllLocals locals_;
  };

  enum
  {
    TASK_RUNNER_STACK_SIZE_ = 1024 * 1024
  };

  typedef std::unique_ptr<GlobalConfig> GlobalConfigPtr;
  typedef std::unique_ptr<class xsd::tests::AutoTests::LocalParamsType> LocalsPtr;
  
public:
  DECLARE_EXCEPTION(Exception, eh::DescriptiveException);
  DECLARE_EXCEPTION(InvalidArgument, Exception);
  DECLARE_EXCEPTION(InvalidOperationOrder, Exception);

public:
  UnitManager();

  ~UnitManager() throw();

  void
  run(int argc, const char* argv[])
    throw(InvalidOperationOrder, Exception, eh::Exception);

  void
  stop(bool result) throw(Exception, eh::Exception);

  bool
  succeed() const throw();

  bool
  active() throw();

private:
  bool
  read_parameters_(int argc, const char* argv[])
    throw(InvalidArgument, Exception, eh::Exception);

  void
  init_()
    throw(InvalidArgument, Exception, eh::Exception);

  void
  print_results()
    throw(eh::Exception);

  void
  run_serialized_(
    const TestFactory::UnitsList& serialized);

  UnitStat*
  add_stat() throw(eh::Exception);

private:
  typedef std::vector<UnitStat_var> Stats;
  typedef Sync::Policy::PosixThread SyncPolicy;

  mutable SyncPolicy::Mutex lock_;
  mutable SyncPolicy::Mutex stat_lock_;
  AutoTest::Logger_var logger_;
  bool active_;
  bool result_;
  Generics::TaskRunner_var task_runner_;
  Generics::Time start_time_;
  Generics::Time stop_time_;
  Stats test_stats_;
  bool sort_performance_;
  std::string config_path_;
  GlobalConfigPtr config_;
  LocalsPtr locals_;
  UnitConfig unit_config;
  TestFactory::TestFactory test_factory_;
  TestFactory::StringList tests_;
  TestFactory::StringList categories_;
  TestFactory::GroupList groups_;
  TestFactory::StringList exclude_tests_;
  TestFactory::StringList exclude_categories_;
  volatile sig_atomic_t no_db_;
};

//
// UnitManager inlines;
//

inline
bool
UnitManager::succeed() const throw()
{
  return result_;
}

inline
bool
UnitManager::active() throw()
{
  return active_;
}

#endif
