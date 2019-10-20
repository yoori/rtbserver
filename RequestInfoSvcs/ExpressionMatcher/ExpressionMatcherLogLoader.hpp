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

#ifndef REQUESTINFOSVCS_EXPRESSIONMATCHER_EXPRESSIONMATCHERLOGLOADER_HPP_
#define REQUESTINFOSVCS_EXPRESSIONMATCHER_EXPRESSIONMATCHERLOGLOADER_HPP_

#include <list>

#include <Generics/CompositeActiveObject.hpp>
#include <Generics/Scheduler.hpp>

#include <LogCommons/FileReceiverFacade.hpp>

#include "ConsiderInterface.hpp"

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class RequestBasicChannelsProcessor
    {
    public:
      virtual
      ~RequestBasicChannelsProcessor() throw ()
      {}

      virtual bool
      process_requests(
        LogProcessing::FileReceiver::FileGuard* file_ptr,
        std::size_t& processed_lines_count)
        throw (eh::Exception) = 0;
    };

    class ExpressionMatcherLogLoader :
      public Generics::CompositeActiveObject,
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      enum class LogType
      {
        RequestBasicChannels,
        ConsiderClick,
        ConsiderImpression
      };

      struct LogReadTraits
      {
        LogType log_type;
        std::string in_path;
        std::string prefix;
        std::string intermediate_fetch_dir;

        LogReadTraits(
          LogType type,
          const std::string& path,
          const std::string& pref,
          const std::string& intermediate)
          throw ()
        : log_type(type), in_path(path), prefix(pref),
          intermediate_fetch_dir(intermediate)
        {}
      };

      typedef std::list<LogReadTraits> LogReadTraitsList;

    public:
      ExpressionMatcherLogLoader(
        ConsiderInterface* consider_interface_,
        RequestBasicChannelsProcessor* request_basic_channels_processor,
        Generics::TaskRunner* task_runner,
        Generics::Planner* scheduler,
        Logging::Logger* logger,
        Generics::ActiveObjectCallback* callback,
        unsigned threads_number,
        LogReadTraitsList log_read_traits,
        unsigned long check_logs_period)
        throw ();

    private:
      typedef LogProcessing::FileReceiverFacade<
        LogProcessing::DefaultOrderStrategy<LogType> > FileReceiverFacade;
      typedef ReferenceCounting::SmartPtr<FileReceiverFacade>
        FileReceiverFacade_var;

      typedef void (ExpressionMatcherLogLoader::*ConsiderAction) (
        const Generics::MemBuf&, uint32_t);

    private:
      ConsiderInterface* consider_interface_;
      RequestBasicChannelsProcessor* request_basic_channels_processor_;
      Generics::TaskRunner_var task_runner_;
      Generics::Planner_var scheduler_;
      Logging::Logger_var logger_;
      FileReceiverFacade_var file_receiver_facade_;
      LogReadTraitsList log_read_traits_;

    private:
      virtual
      ~ExpressionMatcherLogLoader() throw ()
      {}

      void
      fetch_files_(
        const LogReadTraits& log_traits,
        unsigned long check_logs_period,
        LogProcessing::FileReceiver_var file_receiver)
        throw ();

      void
      process_file_() throw ();

      void
      prepare_mem_buf_(
        Generics::MemBuf& membuf,
        unsigned long size)
        throw ();

      bool
      process_binary_file_(
        LogProcessing::FileReceiver::FileGuard* file_ptr,
        ConsiderAction action,
        std::size_t& processed_lines_count)
        throw (eh::Exception);

      void
      consider_click_(
        const Generics::MemBuf& membuf,
        uint32_t size) throw ();

      void
      consider_impression_(
        const Generics::MemBuf& membuf,
        uint32_t size) throw ();

      const LogReadTraits&
      find_log_read_traits_(LogType log_type) throw (Exception);

      void
      file_move_back_to_input_dir_(
        const AdServer::LogProcessing::LogFileNameInfo& info,
        const char* file_name) throw (eh::Exception);
    };

    typedef ReferenceCounting::SmartPtr<ExpressionMatcherLogLoader>
      ExpressionMatcherLogLoader_var;
  }
}

#endif /* REQUESTINFOSVCS_EXPRESSIONMATCHER_EXPRESSIONMATCHERLOGLOADER_HPP_ */
