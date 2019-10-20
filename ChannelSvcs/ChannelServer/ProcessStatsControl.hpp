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

#ifndef CHANNEL_SVCS_PROCESS_STAT_CONTROL
#define CHANNEL_SVCS_PROCESS_STAT_CONTROL

#include <ReferenceCounting/DefaultImpl.hpp>
#include <ReferenceCounting/ReferenceCounting.hpp>
#include <eh/Exception.hpp>
#include <CORBACommons/Stats_s.hpp>

#include "ChannelServerImpl.hpp"

namespace AdServer
{
namespace ChannelSvcs
{
  class ChannelServerStatsImpl:
    public virtual CORBACommons::ReferenceCounting::ServantImpl
      <POA_CORBACommons::ProcessStatsControl>
  {
  public:
    ChannelServerStatsImpl(ChannelServerCustomImpl* delegate)
      throw(eh::Exception);

    virtual ~ChannelServerStatsImpl() throw() {};

    virtual CORBACommons::StatsValueSeq* get_stats()
      throw(CORBACommons::ProcessStatsControl::ImplementationException);

  private:

    ChannelServerCustomImpl_var delegate_;
  };

  typedef ReferenceCounting::SmartPtr<ChannelServerStatsImpl>
    ChannelServerStatsImpl_var;
}
}

#endif //CHANNEL_SVCS_PROCESS_STAT_CONTROL

