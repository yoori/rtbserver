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

#include "BillStatSource.hpp"

namespace AdServer
{
namespace CampaignSvcs
{
  void
  BillStatSource::Stat::AmountDistribution::print(
    std::ostream& out,
    const char* offset)
    const throw()
  {
    out << offset <<
      "before " << prev_day.get_gm_time().format("%F") <<
      ": " << prev_days_amount << std::endl;

    for(auto day_it = day_amounts.begin();
      day_it != day_amounts.end(); ++day_it)
    {
      out << offset <<
        "at " << day_it->first.get_gm_time().format("%F") <<
        ": " << day_it->second << std::endl;
    }
  }

  void
  BillStatSource::Stat::print(std::ostream& out, const char* offset) const
    throw()
  {
    const std::string sub_offset = std::string(offset) + "  ";

    out << offset << "timestamp: " << timestamp.get_gm_time() << std::endl <<
      offset << "accounts:" << std::endl;
    for(auto acc_it = accounts.begin();
        acc_it != accounts.end(); ++acc_it)
    {
      out << offset << acc_it->first << ":" << std::endl;
      acc_it->second.print(out, sub_offset.c_str());
    }

    out << offset << "campaigns: " << std::endl;
    for(auto cmp_it = campaigns.begin();
      cmp_it != campaigns.end(); ++cmp_it)
    {
      out << offset << cmp_it->first << ":" << std::endl;
      cmp_it->second.print(out, sub_offset.c_str());
    }

    out << offset << "ccgs: " << std::endl;
    for(auto ccg_it = ccgs.begin();
      ccg_it != ccgs.end(); ++ccg_it)
    {
      out << offset << ccg_it->first << ":" << std::endl;
      ccg_it->second.print(out, sub_offset.c_str());
    }
  }
}
}
