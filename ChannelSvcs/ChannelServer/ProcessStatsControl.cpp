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

#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <CORBACommons/Stats_s.hpp>
#include "ProcessStatsControl.hpp"
#include "ChannelServerImpl.hpp"

namespace AdServer
{
namespace ChannelSvcs
{
  ChannelServerStatsImpl::ChannelServerStatsImpl(
    ChannelServerCustomImpl* delegate) throw(eh::Exception) :
    delegate_(ReferenceCounting::add_ref(delegate))
  {
  }

  CORBACommons::StatsValueSeq* ChannelServerStatsImpl::get_stats()
    throw(CORBACommons::ProcessStatsControl::ImplementationException)
  {
    const char* FUN = "ChannelServerStatsImpl::get_stats";
    CORBACommons::StatsValueSeq_var stats_seq =
      new CORBACommons::StatsValueSeq;
    if(delegate_.in() != 0)
    {
      AdServer::ChannelSvcs::ChannelServerStats stats;
      delegate_->get_stats(stats);
      try
      {
        size_t i;
        stats_seq->length(AdServer::ChannelSvcs::ChannelServerStats::PARAMS_MAX + 2);
        for(i = 0; i < AdServer::ChannelSvcs::ChannelServerStats::PARAMS_MAX;
            i++)
        {
          (*stats_seq)[i].key =
            AdServer::ChannelSvcs::ChannelServerStats::param_name[i];
          (*stats_seq)[i].value <<= (CORBA::ULong)stats.params[i];
        }
        (*stats_seq)[i].key = "Configuration";
        (*stats_seq)[i++].value <<= stats.configuration;
        (*stats_seq)[i].key = "ConfigurationDate";
        (*stats_seq)[i].value <<= stats.configuration_date.gm_ft();
      }
      catch(const eh::Exception &e)
      {
        Stream::Error ostr;
        ostr << FUN << ": caught eh::Exception."
          " : " << e.what();
        CORBACommons::throw_desc<
          CORBACommons::ProcessStatsControl::ImplementationException>(
            ostr.str());
      }
      catch(const CORBA::SystemException& e)
      {
        Stream::Error ostr;
        ostr << FUN << ": caught CORBA::SystemException."
         " : " << e;
        CORBACommons::throw_desc<
          CORBACommons::ProcessStatsControl::ImplementationException>(
           ostr.str());
      }
    }
    return stats_seq._retn();
  }

}
}
