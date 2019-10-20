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

/// @file AdFrontend/OptOutFrontendStat.hpp
#ifndef OPT_OUT_FRONTENDSTAT_HPP
#define OPT_OUT_FRONTENDSTAT_HPP

#include <eh/Exception.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <Frontends/FrontendCommons/FrontendStatistic.hpp>

namespace AdServer
{
  enum OptOperation
  {
    OO_NOT_DEFINED = 0,
    OO_OUT = 1,
    OO_IN  = 2,
    OO_STATUS
  };

  class OptOutFrontendStat: public FrontendStat
  {
    struct StatData
    {
      unsigned long opt_in_user;
      unsigned long non_opt_on_user;
      unsigned long opt_status;

      StatData&
      operator +=(const StatData& rhs) throw ();
    };

  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    OptOutFrontendStat(
      Logging::Logger* logger,
      const CORBACommons::CorbaObjectRef& stats_collector_ref,
      Generics::Planner* shep_ptr,
      const Generics::Time& dump_period,
      Generics::ActiveObjectCallback* callback,
      const char* host_name = 0)
      throw(Exception);

    void consider_request(OptOperation op) throw();

    virtual Generics::Values_var
    extract_stats_values();

  private:
    StatData stat_data_;
    Sync::PosixMutex mutex_;
  };

  typedef ReferenceCounting::SmartPtr<OptOutFrontendStat>
    OptOutFrontendStat_var;


}

#endif
