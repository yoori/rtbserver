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

#ifndef _USER_INFO_SVCS_CHANNEL_MATCHER_IPP_
#define _USER_INFO_SVCS_CHANNEL_MATCHER_IPP_

namespace AdServer
{
namespace UserInfoSvcs
{
  inline
  void ChannelsMatcher::delete_excess_timestamps_(
    SessionMatchesWriter::timestamps_Container& wr,
    const ChannelIntervalList& cil) throw()
  {
    unsigned long max_vis = cil.max_visits();
    unsigned long min_window_size = cil.min_window_size();
      
    if(max_vis == 0)
    {
      wr.clear();
      return;
    }

    SessionMatchesWriter::timestamps_Container::
      iterator window_end_it = wr.begin();
    SessionMatchesWriter::timestamps_Container::
      iterator window_begin_it = window_end_it;
    SessionMatchesWriter::timestamps_Container::
      iterator erase_candidate_it = window_end_it;
    unsigned long window_imps = 0;
  
    for(; window_end_it != wr.end(); ++window_end_it, ++window_imps)
    {
      while(static_cast<unsigned long>(*window_end_it - *window_begin_it) >
            min_window_size)
      {
        // offset window begin
        --window_imps;
        ++window_begin_it;
        erase_candidate_it = window_begin_it;
      }

      if(window_imps > 2*max_vis) // > max_vis without partly match
      {
        ++erase_candidate_it;
      }
    
      if(window_imps >= 4*max_vis) // >= 2*max_vis without partly match
      {
        // clear middle element
        wr.erase(erase_candidate_it--);
        --window_imps;
      }
    }
  }
}
}

#endif /*_USER_INFO_SVCS_CHANNEL_MATCHER_IPP_*/
