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

#ifndef _USER_INFO_SVCS_USER_INFO_MANAGER_LOGGER_HPP_
#define _USER_INFO_SVCS_USER_INFO_MANAGER_LOGGER_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>

#include <LogCommons/LogHolder.hpp>

#include <LogCommons/ChannelCountStat.hpp>

namespace AdServer
{
  namespace UserInfoSvcs
  {
    class UserInfoManagerLogger:
      public virtual AdServer::LogProcessing::CompositeLogHolder
    {
    public:
      struct HistoryOptimizationInfo
      {
        HistoryOptimizationInfo()
          : colo_id(0),
            isp_date(Generics::Time::ZERO),
            adv_channel_count(0),
            discover_channel_count(0)
        {}
        
        unsigned long colo_id;
        Generics::Time isp_date;
        unsigned long adv_channel_count;
        unsigned long discover_channel_count;
      };

    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      UserInfoManagerLogger(
        const AdServer::LogProcessing::LogFlushTraits& flush_traits)
        throw(Exception);
      
      virtual ~UserInfoManagerLogger() throw();

      void process_history_optimization(
        const HistoryOptimizationInfo& request_info)
        throw(Exception);

    public:
      class HistoryOptimizationStatLogger;
      typedef
        ReferenceCounting::SmartPtr<HistoryOptimizationStatLogger>
        HistoryOptimizationStatLogger_var;
      
    private:
      HistoryOptimizationStatLogger_var history_optimization_stat_logger_;

    private:  
      unsigned long power2align_(unsigned long value) throw();
    };

    typedef ReferenceCounting::SmartPtr<UserInfoManagerLogger>
      UserInfoManagerLogger_var;
  }
}

#endif /*_USER_INFO_SVCS_USER_INFO_MANAGER_LOGGER_HPP_*/
