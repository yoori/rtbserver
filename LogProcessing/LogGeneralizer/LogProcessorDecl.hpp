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

#ifndef AD_SERVER_LOG_PROCESSING_LOG_PROCESSOR_DECL_HPP
#define AD_SERVER_LOG_PROCESSING_LOG_PROCESSOR_DECL_HPP


#include <string>

#include <ReferenceCounting/ReferenceCounting.hpp>
#include <Logger/Logger.hpp>
#include <Sync/SyncPolicy.hpp>
#include <LogCommons/FileReceiver.hpp>

namespace AdServer {
namespace LogProcessing {

class LogProcessor: public ReferenceCounting::AtomicImpl
{
public:
  virtual
  void
  check_and_load() = 0;

  virtual
  const FileReceiver_var&
  get_file_receiver() = 0;

protected:
  virtual ~LogProcessor() throw() {}
};

typedef ReferenceCounting::SmartPtr<LogProcessor> LogProcessor_var;

class LogProcessorImplBase: public LogProcessor
{
public:
  LogProcessorImplBase(
    const std::string &in_dir,
    Logging::Logger *logger
  )
  :
    in_dir_(in_dir),
    logger_(ReferenceCounting::add_ref(logger))
  {
  }

protected:
  virtual ~LogProcessorImplBase() throw() {}

  const std::string in_dir_;
  Logging::Logger_var logger_;
};

} // namespace LogProcessing
} // namespace AdServer

#endif /* AD_SERVER_LOG_PROCESSING_LOG_PROCESSOR_DECL_HPP */
