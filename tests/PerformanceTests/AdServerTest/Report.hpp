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
#include <eh/Exception.hpp>
#include <tests/PerformanceTests/Commons/ReportCommons.hpp>

/**
 * @class Report
 * @brief Base report
 */
class Report
{
public:

  /**
   * @brief Constructor.
   * @param statistics ref
   * @param output stream
   */
  Report(Statistics& stats,
         std::ostream& out);
  
  /**
   * @brief Destructor
   */
  virtual ~Report() throw();

  /**
   * @brief Publish report
   */
  virtual void publish() = 0;

  /**
   * @brief Dump report to output stream
   */
  virtual void dump();

protected:
  Statistics& stats_;
  std::ostream& out_;

};

/**
 * @class DumpRequestContainerConfStats
 * @brief Class for dumping counters container with per/second info
 */
class DumpRequestContainerConfStats : public DumpInterface
{
public:
 
  /**
   * @brief Constructor.
   * @param statistics ref
   */
   DumpRequestContainerConfStats(const RequestCounterContainer& container);

  /**
   * @brief Destructor
   */
  ~DumpRequestContainerConfStats() throw();

protected:
  /**
   * @brief dump body
   * @param output stream
   */  
  virtual std::ostream& dump_body(std::ostream& out) const;  
  
private:
  const RequestCounterContainer& container_;
};


/**
 * @class DumpFrontendStats
 * @brief Class for dumping frontend statistics
 */
class DumpFrontendStats : public DumpInterface
{
public:
  static const unsigned short INDENTION = 30;

  static const unsigned short LINE_LENGTH = 120;
  
public:
   /**
   * @brief Constructor.
   * @param statistics ref
   */
  DumpFrontendStats(const FrontendStatList& items);

  /**
   * @brief Destructor
   */
  ~DumpFrontendStats() throw();

protected:

  /**
   * @brief dump header
   * @param output stream
   */  
  virtual std::ostream& dump_header(std::ostream& out) const;

  /**
   * @brief dump body
   * @param output stream
   */  
  virtual std::ostream& dump_body(std::ostream& out) const;

  
private:
  const FrontendStatList& items_;   
};

/**
 * @class DumpAdvertisingStats
 * @brief Class for dumping advertising statistics
 */
class  DumpAdvertisingStats : public DumpInterface
{

public:
  static const unsigned short INDENTION = 30;
  
  static const unsigned short LINE_LENGTH = 120;

public:
  
   /**
   * @brief Constructor.
   * @param statistics ref
   */
   DumpAdvertisingStats(const AdvertisingStatList& items);

  /**
   * @brief Destructor
   */
  ~DumpAdvertisingStats() throw();

protected:

  /**
   * @brief dump body
   * @param output stream
   */  
  virtual std::ostream& dump_body(std::ostream& out) const;
  
private:
  const AdvertisingStatList& items_;

private:
  std::ostream& dump_ccid_header(std::ostream& out) const;

  std::ostream& dump_ccid_body(std::ostream& out,
                               const AdvertisingStatistics_var& stats) const;

};

/**
 * @class ShortReport
 * @brief Short statistics form report
 */
class ShortReport : public Report
{
public:
  /**
   * @brief Constructor.
   * @param statistics ref
   * @param output stream
   */
  ShortReport(Statistics& stats,
              std::ostream& out);

  /**
   * @brief Destructor
   */
  ~ShortReport() throw();

  /**
   * @brief Publish report
   */
  virtual void publish();

  /**
   * @brief dump
   */  
  virtual void dump();
   
};

/**
 * @class ChannelsDump
 * @brief Channels statistics report
 */
class ChannelsReport : public  Report
{
public:
  /**
   * @brief Constructor.
   * @param statistics ref
   * @param output stream
   */
  ChannelsReport(Statistics& stats,
                 std::ostream& out);

  /**
   * @brief Destructor
   */
  ~ChannelsReport() throw();

  /**
   * @brief Publish report
   */
  virtual void publish();


  /**
   * @brief dump
   */  
  virtual void dump();  
 
};


/**
 * @class ChannelsReport
 * @brief Channels statistics report
 */
class DiscoverReport : public Report
{
public:
  /**
   * @brief Constructor.
   * @param statistics ref
   * @param output stream
   */
  DiscoverReport(Statistics& stats,
                 std::ostream& out);

  /**
   * @brief Destructor
   */
  ~DiscoverReport() throw();

  /**
   * @brief dump
   */  
  virtual void publish();

  /**
   * @brief dump
   */  
  virtual void dump();  
 
};


/**
 * @class StandardReport
 * @brief Using for statistics logging
 */
class StandardReport : public Report
{
public:

  /**
   * @brief Constructor.
   * @param statistics ref
   * @param output stream
   */
  StandardReport(Statistics& stats,
                 std::ostream& out);

  /**
   * @brief Destructor
   */
  ~StandardReport() throw();

  /**
   * @brief dump
   */  
  virtual void publish();  

private:
  
  virtual std::ostream& dump_frontend_footer(std::ostream& out);
 
};

/**
 * @class DumpPerformanceStats
 * @brief Class for dumping performance statistics
 */
class DumpConfPerformanceStats : public DumpInterface
{
 
public:

  /**
   * @brief Constructor.
   * @param statistics ref
   * @param performance statistics header
   */
  DumpConfPerformanceStats(const PerformanceStatisticsBase& stats,
                           const char* header);

  /**
   * @brief Destructor
   */
  ~DumpConfPerformanceStats() throw();

protected:

  /**
   * @brief dump header
   * @param output stream
   */  
  virtual std::ostream& dump_header(std::ostream& out) const;

  /**
   * @brief dump body
   * @param output stream
   */  
  virtual std::ostream& dump_body(std::ostream& out) const;  
  
private:
  const PerformanceStatisticsBase& stats_;
  std::string header_;
};


class ConfluenceReport : public Report
{
public:

  DECLARE_EXCEPTION(ConfReportError, eh::DescriptiveException);      
  /**
   * @brief Constructor.
   * @param statistics ref
   * @param report name
   * @param report file path
   */
  ConfluenceReport(Statistics& stats,
                   const Configuration& config,
                   const ConstraintsContainer& constraints,
                   Generics::Time total_duration);

  /**
   * @brief Destructor
   */
  ~ConfluenceReport() throw();

  /**
   * @brief dump
   */  
  virtual void publish();

private:
  const Configuration& config_;
  const ConstraintsContainer& constraints_;
  Generics::Time total_duration_;
  std::ofstream file_;
  
private:
  bool is_file_exist();
  void dump_header(std::ostream& out);
  void dump_body(std::ostream& out);
  void dump_server_list(std::ostream& out);
};



#endif  // _REPORT_HPP
