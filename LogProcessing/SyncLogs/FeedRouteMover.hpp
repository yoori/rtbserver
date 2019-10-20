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

#ifndef LOGPROCESSING_SYNCLOGS_FEEDROUTEMOVER_HPP_
#define LOGPROCESSING_SYNCLOGS_FEEDROUTEMOVER_HPP_

#include <ReferenceCounting/AtomicImpl.hpp>
#include <Generics/ActiveObject.hpp>

#include "Utils.hpp"
#include "RouteProcessor.hpp"
#include "FileRouter.hpp"
#include "RouteHelpers.hpp"

namespace AdServer
{
namespace LogProcessing
{
  class FeedRouteMover:
    public ReferenceCounting::AtomicImpl,
    public Generics::SimpleActiveObject,
    protected InterruptCallback
  {
  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    FeedRouteMover(
      Utils::ErrorPool* error_logger,
      const LocalInterfaceChecker& host_checker,
      const char* dst_dir,
      unsigned long tries_per_file,
      RouteBasicHelper* route_helper,
      FileRouter* local_file_router,
      FileRouter* remote_file_router,
      bool parse_source,
      bool unlink_source,
      bool interruptible,
      bool commit_mode_flag)
      throw (Exception);

    bool
    move(
      const char* src_path,
      const char* src_file,
      std::string* dst_host = nullptr)
      throw(Utils::UnlinkException);

    bool
    interrupt() throw(eh::Exception);

  protected:
    virtual
    ~FeedRouteMover() throw()
    {}

  private:
    struct TraceLevel
    {
      enum
      {
        LOW = Logging::Logger::TRACE,
        MIDDLE,
        HIGH
      };
    };

  private:
    // const configuration
    Utils::ErrorPool* const error_logger_;

    const LocalInterfaceChecker& host_checker_;
    const std::string src_files_pattern_;
    std::string dst_dir_;

    const bool parse_source_;
    const bool unlink_source_;
    const bool interruptible_;

    const unsigned long tries_per_file_;

    FixedRouteBasicHelper_var route_helper_;
    FixedFileRouter_var local_file_router_;
    FixedFileRouter_var remote_file_router_;
    bool commit_mode_flag_;
  };

  typedef ReferenceCounting::SmartPtr<FeedRouteMover>
    FeedRouteMover_var;
  typedef ReferenceCounting::FixedPtr<FeedRouteMover>
    FixedFeedRouteMover_var;
}
}

#endif /* LOGPROCESSING_SYNCLOGS_FEEDROUTEMOVER_HPP_ */
