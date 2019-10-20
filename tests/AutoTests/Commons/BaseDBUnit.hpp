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

#ifndef __AUTOTESTS_COMMONS_BASEDBUNIT_HPP
#define __AUTOTESTS_COMMONS_BASEDBUNIT_HPP

#include <tuple>
#include <Commons/PathManip.hpp>
#include <tests/AutoTests/Commons/BaseUnit.hpp>
#include <tests/AutoTests/Commons/ORM/ORM.hpp>

/**
 * @class BaseDBUnit
 * @brief Test class with db object
 */
class BaseDBUnit:
  public BaseUnit
{
public:
  /**
   * @brief Constructor.
   * @param test statistics.
   * @param test name.
   * @param test config.
   */
  BaseDBUnit(
    UnitStat& stat_var,
    const char* task_name,
    XsdParams params_var);

  /**
   * @brief Destructor.
   */
  virtual ~BaseDBUnit() throw();

  /**
   * @brief Test setup.
   * Use to prepare test data, check environment, etc.
   */  
  virtual void set_up();

  /**
   * @brief Test tear down.
   * Use to restore DB entities, remove statistics, etc.
   */  
  virtual void tear_down() = 0;

  /**
   * @brief Test pre condition.
   * Use to check initial state.
   */  
  virtual void pre_condition();

  /**
   * @brief Run test cases.
   */  
  virtual bool run() = 0;

  /**
   * @brief Test pre condition.
   * Use to check result state.
   */  
  virtual void post_condition();

  /**
   * @brief Run test.
   */  
  bool run_test();

  /**
   * @brief Create DB entity.
   * @param entity ID.
   * @return restorer entity wrapper
   */  
  template <typename Entity>
  AutoTest::ORM::ORMRestorer<Entity>*
  create(unsigned long id) throw(eh::Exception);

  /**
   * @brief Create DB entity.
   * @param entity.
   * @return restorer entity wrapper
   */  
  template <typename Entity>
  AutoTest::ORM::ORMRestorer<Entity>*
  create(const Entity& e) throw(eh::Exception);

  /**
   * @brief Create DB entity.
   * @return restorer entity wrapper
   */  
  template <typename Entity>
  AutoTest::ORM::ORMRestorer<Entity>*
  create() throw(eh::Exception);

protected:
  DECLARE_EXCEPTION(DataBaseError, eh::DescriptiveException);

  AutoTest::DBC::Conn pq_conn_; // Postgres connection
  AutoTest::ORM::Restorers restorers_; // ORM restorers storage

private:
  
  void safe_tear_down();

  AutoTest::DBC::Conn&
  get_conn(
    AutoTest::ORM::postgres_connection conn_type);
};

#include "BaseDBUnit.ipp"

#endif  // __AUTOTESTS_COMMONS_BASEDBUNIT_HPP
