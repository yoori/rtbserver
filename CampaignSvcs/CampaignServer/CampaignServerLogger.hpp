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

#ifndef _AD_SERVER_CAMPAIGN_SVCS_CAMPAIGNSERVERLOGGER_HPP_
#define _AD_SERVER_CAMPAIGN_SVCS_CAMPAIGNSERVERLOGGER_HPP_

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Sync/SyncPolicy.hpp>

#include <LogCommons/ColoUpdateStat.hpp>

namespace AdServer
{
  namespace CampaignSvcs
  {
    struct LogFlushTraits
    {
      unsigned long size;
      unsigned long period;
      std::string out_dir;
    };

    class CampaignServerLogger:
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      struct ConfigUpdateInfo
      {
        unsigned long colo_id;
        Generics::Time time;
        std::string version;
      };
      
    public:
      CampaignServerLogger(const LogFlushTraits& colo_update_log_params)
        throw(Exception);

      void process_config_update(
        const ConfigUpdateInfo& request_info)
        throw(Exception);

      void flush_if_required() throw(Exception);

    protected:
      virtual ~CampaignServerLogger() throw()
      {}

      bool need_flush_i() const throw(eh::Exception);

    protected:
      typedef Sync::Policy::PosixThread SyncPolicy;
      typedef
        AdServer::LogProcessing::ColoUpdateStatTraits::CollectorType
        ColoUpdateCollector;
      typedef
        AdServer::LogProcessing::GenericLogIoHelperImpl<
          AdServer::LogProcessing::ColoUpdateStatTraits>
        ColoUpdateStatIoHelper;

      LogFlushTraits colo_update_flush_traits_;
      SyncPolicy::Mutex colo_update_lock_;
      ColoUpdateCollector colo_update_collector_;
      Generics::Time flush_time_;
    };

    typedef
      ReferenceCounting::SmartPtr<CampaignServerLogger>
      CampaignServerLogger_var;
  }
}

#endif /*_AD_SERVER_CAMPAIGN_SVCS_CAMPAIGNSERVERLOGGER_HPP_*/
