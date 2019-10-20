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

/// @file AdFrontend/AdFrontendStat.hpp
#ifndef ADFRONTENDSTAT_HPP
#define ADFRONTENDSTAT_HPP

#include <eh/Exception.hpp>
#include <Generics/CompositeActiveObject.hpp>
#include <CORBACommons/CorbaAdapters.hpp>
#include <Frontends/FrontendCommons/FrontendStatistic.hpp>
#include "RequestInfoFiller.hpp"

namespace AdServer
{
  struct RequestTimeMetering
  {
    RequestTimeMetering();

    Generics::Time trigger_match_time;
    Generics::Time full_match_time;
    Generics::Time request_fill_time;
    Generics::Time history_match_time;
    Generics::Time history_match_local_time;
    Generics::Time creative_selection_time;
    Generics::Time creative_selection_local_time;
    Generics::Time history_post_match_time;
    Generics::Time merge_users_time;
    std::vector<Generics::Time> detail_trigger_match_time;
    unsigned long recived_triggers;
    unsigned long matched_triggers;
    unsigned long matched_channels;
    unsigned long creative_count;
    bool passback;
    bool profiling;
  };

  class AdFrontendStat: public FrontendStat
  {
    struct BaseStatData
    {
      BaseStatData();
      BaseStatData(
        unsigned long adRequest_,
        unsigned long adRequestWithImpressionOptInUser_,
        unsigned long adRequestWithImpressionnonOptInUser_,
        unsigned long adRequestWithPassbackOptInUser_,
        unsigned long adRequestWithPassbacknonOptInUser_,
        unsigned long campaignSelectOptOutUser_,
        unsigned long campaignSelectOptInUser_,
        unsigned long campaignSelectProbeUser_,
        unsigned long campaignSelectTemporaryUser_,
        unsigned long campaignSelectUndefinedUser_,
        unsigned long passbackRequest_,
        unsigned long profilingRequestReceivedTriggers_,
        unsigned long profilingRequestMatchedTriggers_,
        unsigned long profilingRequestTriggeredChannels_,
        unsigned long profilingRequest_);

      unsigned long adRequest;
      unsigned long adRequestWithImpressionOptInUser;
      unsigned long adRequestWithImpressionnonOptInUser;
      unsigned long adRequestWithPassbackOptInUser;
      unsigned long adRequestWithPassbacknonOptInUser;
      unsigned long campaignSelectOptOutUser;
      unsigned long campaignSelectOptInUser;
      unsigned long campaignSelectProbeUser;
      unsigned long campaignSelectTemporaryUser;
      unsigned long campaignSelectUndefinedUser;
      unsigned long passbackRequest;
      unsigned long profilingRequestReceivedTriggers;
      unsigned long profilingRequestMatchedTriggers;
      unsigned long profilingRequestTriggeredChannels;
      unsigned long profilingRequest;

      BaseStatData&
      operator +=(const BaseStatData& rhs) throw ();
    };

    struct StatData: BaseStatData
    {
      StatData(): BaseStatData(), timeStatCounter(0) {}

      unsigned long timeStatCounter;
      Generics::Time historyMatchTotal;
      Generics::Time historyMatchMin;
      Generics::Time historyMatchMax;
      Generics::Time usersMergeTotal;
      Generics::Time usersMergeMin;
      Generics::Time usersMergeMax;
      Generics::Time triggersMatchTotal;
      Generics::Time triggersMatchMin;
      Generics::Time triggersMatchMax;
      Generics::Time requestParsingTotal;
      Generics::Time requestParsingMin;
      Generics::Time requestParsingMax;
      Generics::Time creativeSelectionTotal;
      Generics::Time creativeSelectionMin;
      Generics::Time creativeSelectionMax;
      Generics::Time creativeSelectionLocalTotal;
      Generics::Time creativeSelectionLocalMin;
      Generics::Time creativeSelectionLocalMax;

      static void
      process_time(const Generics::Time& time, Generics::Time& total, Generics::Time& min, Generics::Time& max) throw ();
    };

  public:
    DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

    AdFrontendStat(
      Logging::Logger* logger,
      const CORBACommons::CorbaObjectRef& stats_collector_ref,
      Generics::Planner* shep_ptr,
      const Generics::Time& dump_period,
      Generics::ActiveObjectCallback* callback,
      const char* host_name = 0)
      throw(Exception);

    void consider_request(
      const RequestInfo& request,
      const RequestTimeMetering& request_times)
      throw();

    virtual Generics::Values_var
    extract_stats_values();

  protected:
    virtual ~AdFrontendStat() throw()
    {}

  private:
    StatData stat_data_;
    Sync::PosixMutex mutex_;
  };

  typedef ReferenceCounting::SmartPtr<AdFrontendStat>
    AdFrontendStat_var;

  inline
  RequestTimeMetering::RequestTimeMetering()
    : recived_triggers(0),
      matched_triggers(0),
      matched_channels(0),
      creative_count(0),
      passback(true),
      profiling(false)
  {
  }
}

#endif
