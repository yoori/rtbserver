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

#ifndef _UNITTEST__CHANNELSTATUSTEST_
#define _UNITTEST__CHANNELSTATUSTEST_


#include <tests/AutoTests/Commons/Common.hpp>

namespace DB = ::AutoTest::DBC;
namespace ORM = ::AutoTest::ORM;

/**
 * @class ChannelStatusTest
 * @brief tests dynamic channels loading
 */ 
class ChannelStatusTest
  :public BaseDBUnit
{

public:

  ChannelStatusTest(
      UnitStat& stat_var,
      const char* task_name,
      XsdParams params_var):
    BaseDBUnit(stat_var, task_name, params_var)
  {};

  virtual ~ChannelStatusTest() throw()
  {}

private:

  template<typename ChannelChecker = AutoTest::SimpleChannelChecker>
  class ChannelStatusChecker: public AutoTest::Checker
  {
  public:

    ChannelStatusChecker(
      BaseUnit* test,
      const std::string& prefix,
      const std::string& status,
      bool wait = false,
      bool update_stats = false);

    ~ChannelStatusChecker() throw();

    bool check(bool throw_error = true)
      throw (AutoTest::CheckFailed, eh::Exception);

  private:

    void init_();

    bool request_check_(bool throw_error = true)
      throw (AutoTest::CheckFailed, eh::Exception);

    BaseUnit* test_;
    std::string prefix_;
    std::string status_;
    bool wait_;
    bool update_stats_;
  };

  class ReachChannelThresholdChecker: public AutoTest::Checker
  {
    typedef AutoTest::ORM::ChannelInventoryStats Stats;
  public:
    ReachChannelThresholdChecker(
      DB::IConn& connection,
      unsigned long channel_id,
      int threshold_value);

    ~ReachChannelThresholdChecker() throw();

    bool check(bool throw_error = true)
      throw (AutoTest::CheckFailed, eh::Exception);

  private:
    DB::IConn& conn_;
    Stats stats_;
    int threshold_value_;
  };

  private:

  void change_channel_status();
  void channel_threshold_feature();

  void set_up();
  void pre_condition();
  bool run();
  void post_condition();
  void tear_down();
  
  
};
 
#endif
