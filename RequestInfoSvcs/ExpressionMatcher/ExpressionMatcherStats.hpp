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

/// @file ExpressionMatcherStats.hpp

#ifndef EXPRESSION_MATCHER_STATS_INCLUDED
#define EXPRESSION_MATCHER_STATS_INCLUDED

#include <Commons/AtomicInt.hpp>
#include <Generics/Values.hpp>
#include <Generics/Time.hpp>
#include <Sync/SyncPolicy.hpp>

namespace AdServer
{
  namespace RequestInfoSvcs
  {
    struct Stats
    {
      Stats() throw ();

      Algs::AtomicInt processed_matches_optin_user;
      Algs::AtomicInt processed_matches_temporary_user;
      Algs::AtomicInt processed_matches_non_optin_user;
      Generics::Time last_processed_file_timestamp;
    };

    class StatsCounters : private Stats
    {
    public:
      void
      inc_not_optedin_user_processed() throw ();

      void
      inc_temporary_user_processed() throw ();

      void
      inc_persistent_user_processed() throw ();

      void
      set_last_processed_timestamp(const Generics::Time& time)
        throw ();

      const Stats&
      get_stats() const throw ();
    };

    class ExpressionMatcherStatsImpl : public Generics::Values
    {
    public:
      void
      fill_values(const Stats& stats) throw (eh::Exception);

    protected:
      virtual
      ~ExpressionMatcherStatsImpl() throw () = default;
    };

    typedef ExpressionMatcherStatsImpl ProcStatImpl;
    typedef ReferenceCounting::SmartPtr<ProcStatImpl>
      ProcStatImpl_var;
  }
}

//
// Inlines impl's
//
namespace AdServer
{
  namespace RequestInfoSvcs
  {
    inline
    Stats::Stats() throw ()
      : processed_matches_optin_user(0),
        processed_matches_temporary_user(0),
        processed_matches_non_optin_user(0)
    {}

    inline void
    StatsCounters::inc_not_optedin_user_processed() throw ()
    {
      processed_matches_optin_user += 1;
    }

    inline void
    StatsCounters::inc_temporary_user_processed() throw ()
    {
      processed_matches_temporary_user += 1;
    }

    inline void
    StatsCounters::inc_persistent_user_processed() throw ()
    {
      processed_matches_non_optin_user += 1;
    }

    inline void
    StatsCounters::set_last_processed_timestamp(const Generics::Time& time)
      throw ()
    {
      last_processed_file_timestamp = time;
    }

    inline const Stats&
    StatsCounters::get_stats() const throw ()
    {
      return *this;
    }
  }
}

#endif // EXPRESSION_MATCHER_STATS_INCLUDED
