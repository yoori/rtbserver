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

#ifndef _EXECUTION_TIME_TRACER_HPP_
#define _EXECUTION_TIME_TRACER_HPP_

#include <Generics/Time.hpp>
#include <Logger/Logger.hpp>

namespace AdServer
{
namespace CampaignSvcs
{
  class ExecutionTimeTracer
  {
  public:
    ExecutionTimeTracer(const char* fun,
      const char* aspect,
      Logging::Logger* logger,
      const char* operation = "")
      : fun_(fun),
        aspect_(aspect),
        operation_(operation[0] ? (std::string(operation) + " ") :
          std::string()),
        logger_(ReferenceCounting::add_ref(logger))
    {
      timer.start();
    }

    virtual ~ExecutionTimeTracer() throw()
    {
      timer.stop();

      logger_->sstream(Logging::Logger::DEBUG,
        aspect_.c_str()) << fun_ <<
        ": " << operation_ << "execution time: " << timer.elapsed_time();
    }
    
  private:
    std::string fun_;
    std::string aspect_;
    std::string operation_;
    Logging::Logger_var logger_;
    Generics::Timer timer;
  };
}
}

#endif
