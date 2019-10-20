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

/// @file RequestInfoManagerStatsAgent.hpp

#ifndef REQUEST_INFO_MANAGER_STATS_AGENT_INCLUDED
#define REQUEST_INFO_MANAGER_STATS_AGENT_INCLUDED

#include <SNMPAgent/SNMPAgentX.hpp>

#include "RequestInfoManagerStats.hpp"


namespace AdServer
{
  namespace RequestInfoSvcs
  {
    class Processor
    {
    public:
      explicit
      Processor(unsigned id)
        throw ();

      void
      register_ids(SNMPAgentX::GenericSNMPAgent* agent) const
        throw (eh::Exception);

      bool
      process_variable(void* variable,
        const SNMPAgentX::GenericSNMPAgent::VariableInfo& info,
        unsigned size, const unsigned* ids,
        const RequestInfoManagerStatsImpl* values) const
        throw (eh::Exception);

    protected:
      /// Will be foros_suffix from config
      unsigned id_;
    };

    typedef SNMPAgentX::SNMPStatsGen<RequestInfoManagerStatsImpl, Processor>
      SNMPStatsImpl;
    typedef ReferenceCounting::QualPtr<SNMPStatsImpl> SNMPStatsImpl_var;
  } // namespace RequestInfoSvcs
} // namespace AdServer

//
// INLINES
//

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    inline
    Processor::Processor(unsigned id) throw ()
      : id_(id)
    {
    }

    inline
    void
    Processor::register_ids(SNMPAgentX::GenericSNMPAgent* agent) const
      throw (eh::Exception)
    {
      if (const SNMPAgentX::GenericSNMPAgent::RootInfo* root =
        agent->get_rootinfo("logStatsTable.logStatsEntry"))
      {
        for (unsigned i = 0; i < StatNames::LOG_COUNT; ++i)
        {
          unsigned ids[] = {i, id_};
          root->register_index(2, ids);
        }
      }
    }

    inline
    bool
    Processor::process_variable(void* variable,
      const SNMPAgentX::GenericSNMPAgent::VariableInfo& info,
      unsigned /*size*/, const unsigned* ids,
      const RequestInfoManagerStatsImpl* values) const
      throw (eh::Exception)
    {
      return SNMPAgentX::GenericSNMPAgent::set_variable_from_values(
        variable, info, *values->get_log_values(*ids));
    }
  } //  namespace RequestInfoSvcs
} // namespace AdServer

#endif // REQUEST_INFO_MANAGER_STATS_AGENT_INCLUDED
