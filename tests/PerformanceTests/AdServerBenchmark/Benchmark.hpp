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

#ifndef _BENCHMARK_HPP
#define _BENCHMARK_HPP

#include <tests/PerformanceTests/Commons/HttpPoolPolicies.hpp>
#include "BenchmarkStorage.hpp"


/**
 * @class BenchmarkBase
 * @brief Benchmark base class
 */
class BenchmarkBase
{
  
public:

  DECLARE_EXCEPTION(NotSupported, eh::DescriptiveException);

  /**
   * @brief Constructor
   *
   * @param config
   * @param storage
   * @param HTTP pool
   * @param logger
   */
  BenchmarkBase(
    const BenchmarkConfig* config,
    BenchmarkStorage* storage,
    HttpInterface* pool,
    Logging::Logger* logger);

  /**
   * @brief Destructor
   *
   */
  virtual ~BenchmarkBase() throw();

  /**
   * @brief Benchmark implementation.
   */
  virtual bool run();

protected:

  /**
   * @brief Send request through client
   *
   * @param client
   */
  virtual void send_request(AdServerClientBase* client);

  /**
   * @brief Send request from storage
   */
  virtual void send_storage_request() = 0;


  /**
   * @brief Make client
   *
   * @param request ID
   * @return server client
   */
  AdServerClientBase* make_client(unsigned long client_id);

  /**
   * @brief Make benchmark request
   *
   * @param client
   * @param request URL
   */
  BaseRequest* make_request(AdServerClientBase* client,
                            const char* url = 0);

protected:
  const BenchmarkConfig* config_;     // benchmark config
  BenchmarkStorage* storage_;         // benchmarks storage
  HttpInterface* pool_;               // HTTP pool
  Logging::Logger* logger_;          // Logger
};


/**
 * @class Benchmark
 * @brief Benchmark with URL creation
 */
class Benchmark : public BenchmarkBase
{
public:

  /**
   * @brief Constructor
   *
   * @param config
   * @param storage
   * @param HTTP pool
   * @param logger
   */
  Benchmark(
    const BenchmarkConfig* config,
    BenchmarkStorage* storage,
    HttpInterface* pool,
    Logging::Logger* logger);
  
  /**
   * @brief Destructor
   *
   */
  virtual ~Benchmark() throw();

protected:

  /**
   * @brief Send request through client
   *
   * @param client
   */
  virtual void send_request(AdServerClientBase* client);

  /**
   * @brief Send request from storage
   */
  virtual void send_storage_request();
  
};


/**
 * @class GeneratedBenchmark
 * @brief Benchmark with generated URL
 */
class GeneratedBenchmark : public BenchmarkBase
{
public:

  /**
   * @brief Constructor
   *
   * @param Application
   * @param Benchmark size (in requests) 
   * @param Opted out benchmark
   * @param request config
   * @param logger
   */
  GeneratedBenchmark(
    const BenchmarkConfig* config,
    BenchmarkStorage* storage,
    HttpInterface* pool,
    Logging::Logger* logger);
  
  /**
   * @brief Destructor
   *
   */
  virtual ~GeneratedBenchmark() throw();

  /**
   * @brief Benchmark implementation.
   */
  virtual bool run();

protected:

  /**
   * @brief Send request from storage
   */
  virtual void send_storage_request();
};

typedef std::unique_ptr<BenchmarkBase> Benchmark_var;

#endif  // _BENCHMARK_HPP
