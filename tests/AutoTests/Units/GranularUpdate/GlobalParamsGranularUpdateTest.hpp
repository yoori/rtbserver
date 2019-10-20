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


#ifndef _UNITTEST__GLOBALPARAMSGRANULARUPDATETEST_
#define _UNITTEST__GLOBALPARAMSGRANULARUPDATETEST_

#include <tests/AutoTests/Commons/Common.hpp>

namespace DB  = ::AutoTest::DBC;
namespace ORM = ::AutoTest::ORM;

/**
* @class GlobalParamsGranularUpdateTest
* @brief Test for global params granular update
*/ 
 
class GlobalParamsGranularUpdateTest: public BaseDBUnit
{
public:
  GlobalParamsGranularUpdateTest(
    UnitStat& stat_var, 
    const char* task_name, 
    XsdParams params_var)
    : BaseDBUnit(stat_var, task_name, params_var),
      currency_exchange_id_(0)
  {};
 
  virtual ~GlobalParamsGranularUpdateTest() throw()
  {};

  void set_up();
  void tear_down();
  bool run();
 
private:
  /**
   * @class GlobalParamsGranularUpdateTest::Exchange
   * @brief Wrapper for CurrencyExchange table
   */
  class Exchange
  {
  public:
    /**
     * @brief Constructor.
     *
     * Select existing CurrencyExchange.
     * @param connection specify DB connection.
     * @param id CurrencyExchange id to select.
     */
    Exchange(DB::IConn& connection,
      unsigned int id);

    /**
     * @brief Constructor.
     *
     * Creates new CurrencyExchane.
     * @param connection specify DB connection
     * @param effective_date currency exchange date
     */
    Exchange(
      DB::IConn& connection,
      const AutoTest::Time& effective_date);

    ~Exchange();

    /**
     * Delete currency exchange.
     */
    bool del();

    /**
     * Return currency exchange id
     */
    const ORM::ORMInt::value_type& id();

  private:
    DB::IConn& connection_;
    ORM::PQ::CurrencyExchange currency_exchange_;

    // add currency exchange rate for existing currencies
    void add_currencies_();

    // delete currency exchange rate for existing currencies
    void delete_currencies_();  
  };

private:
  void check_currency_exchange_();
  void restore_default_values_();

private:
  unsigned int currency_exchange_id_;
  std::string frontend_;
};

#endif 
