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

#ifndef AD_SERVER_CHANNEL_UPDATE_STAT_LOGGER_HPP
#define AD_SERVER_CHANNEL_UPDATE_STAT_LOGGER_HPP

#include <eh/Exception.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <ReferenceCounting/AtomicImpl.hpp>
#include <Sync/SyncPolicy.hpp>

#include <LogCommons/ColoUpdateStat.hpp>

namespace AdServer
{
  namespace ChannelSvcs
  {
    class ChannelUpdateStatLogger:
      public virtual ReferenceCounting::AtomicImpl
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      ChannelUpdateStatLogger(
        unsigned long size,
        unsigned long period, 
        const char* out_dir)
        throw(eh::Exception);
    protected:
      virtual ~ChannelUpdateStatLogger() throw();
    public:

      void process_config_update(
        unsigned long colo_id,
        const std::string& version)
        throw(Exception);

      void flush_if_required() throw(Exception);

    protected:
      bool need_flush_i_() const throw(eh::Exception);

    private:
      typedef Sync::Policy::PosixThread SyncPolicy;
      typedef
        AdServer::LogProcessing::ColoUpdateStatTraits::CollectorType
        ColoUpdateCollector;
      typedef
        AdServer::LogProcessing::GenericLogIoHelperImpl<
          AdServer::LogProcessing::ColoUpdateStatTraits>
        ColoUpdateStatIoHelper;

      SyncPolicy::Mutex colo_update_lock_;
      ColoUpdateCollector colo_update_collector_;
      Generics::Time flush_time_;
      unsigned long size_;
      unsigned long period_;
      std::string out_dir_;
    };

    typedef
      ReferenceCounting::SmartPtr<ChannelUpdateStatLogger>
      ChannelUpdateStatLogger_var;
    
  }
}

#endif//AD_SERVER_CHANNEL_UPDATE_STAT_LOGGER_HPP
