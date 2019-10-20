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


#ifndef _REPORT_HPP
#define _REPORT_HPP

#include "Statistics.hpp"
#include <tests/PerformanceTests/Commons/ReportCommons.hpp>
#include <xsd/tests/PerformanceTests/AdServerBenchmark/AdServerBenchmarkConfig.hpp>


/**
 * @class Report
 * @brief Benchmark report
 */
class Report
{

  static const unsigned long INDENTION=30;

public:

  /**
   * @brief Constructor.
   * @param benchmark description
   * @param frontend type
   * @param statistics reference
   * @param output stream
   */
  Report(const char* description,
         FrontendType frontend_type,
         Statistics& stats,
         std::ostream& out);

  /**
   * @brief Dump report to output stream
   */
  void dump();

private:
  std::string description_;     // description
  FrontendType frontend_type_;  // frontend type
  Statistics& stats_;           // statistics
  std::ostream& out_;           // output stream
};


#endif  // _REPORT_HPP
