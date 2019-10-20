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


#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP

#include <ReferenceCounting/Interface.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/TaskRunner.hpp>
#include <Generics/ActiveObject.hpp>
#include <Logger/StreamLogger.hpp>
#include <Logger/DistributorLogger.hpp>
#include "Benchmark.hpp"
#include "Report.hpp"

/**
 * @class Application
 * @brief AdServerBenchmark test implementation
 */
class Application :
  public Generics::ActiveObjectCallback,
  public ReferenceCounting::AtomicImpl
{

public:

  /**
   * @brief Constructor
   *
   * @param log level
   * @param configuration file path
   */
  Application(unsigned long log_level,
              const char* cfg_file_path);

  /**
   * @brief Destructor
   */
  ~Application() throw();


  /**
   * @brief Init test
   */
  void init() throw(eh::Exception);

  /**
   * @brief Run test
   */
  void run();

  /**
   * @brief Callback calling after error in task runner
   * inherited from ActiveObjectCallback
   *
   * @param severity
   * @param description
   * @param error code
   */
  virtual void report_error(
    Generics::ActiveObjectCallback::Severity severity,
    const String::SubString& description,
    const char* error_code = 0) throw();

private:

  /**
   * @brief Start pools
   */
  void _start();

  /**
   * @brief Stop pools
   */
  void _stop();

private:
  Configuration config_;                      // Configuration
  unsigned long log_level_;                   // log level
  Logging::Logger_var logger_;     // logger
  Generics::TaskRunner_var task_runner_;      // task runner
  HttpPoolPolicy_var http_pool_policy_;       // HTTP pool policies
  HTTP::HttpActiveInterface_var http_pool_;   // HTTP pool
};

#endif  // _APPLICATION_HPP
