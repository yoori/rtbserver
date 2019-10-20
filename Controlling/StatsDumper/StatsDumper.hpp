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

/// @file Controlling/StatsDumper/StatsDumper.hpp
#ifndef CONTROLLING_STATS_DUMPER_HPP_
#define CONTROLLING_STATS_DUMPER_HPP_

#include <eh/Exception.hpp>

#include <CORBACommons/StatsImpl.hpp>
#include <Commons/CorbaObject.hpp>
#include <Controlling/StatsCollector/StatsCollector.hpp>

namespace AdServer
{
  namespace Controlling
  {
    class StatsDumper
    {
    public:
      DECLARE_EXCEPTION(Exception, eh::DescriptiveException);

      StatsDumper(
        const CORBACommons::CorbaObjectRef& ref,
        const char* host_name = 0)
        throw (Exception);

      void
      dump_statistics(const Generics::Values& stat_snapshot)
        throw (Exception);

    private:
      std::string host_name_;
      Commons::CorbaObject<StatsCollector> ref_;
    };

  }
}

#endif //CONTROLLING_STATS_DUMPER_HPP_
