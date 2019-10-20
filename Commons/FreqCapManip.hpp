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

#ifndef _FREQCAPMANIP_HPP_
#define _FREQCAPMANIP_HPP_

#include "FreqCap.hpp"
#include <Commons/FreqCapInfo.hpp>
#include <Commons/CorbaAlgs.hpp>

namespace AdServer
{
  namespace Commons
  {
    inline
    void pack_freq_cap(
      AdServer::Commons::FreqCapInfo& res, const FreqCap& fc)
    {
      res.fc_id = fc.fc_id;
      res.lifelimit = fc.lifelimit;
      res.period = CorbaAlgs::pack_time(fc.period);
      res.window_limit = fc.window_limit;
      res.window_time = CorbaAlgs::pack_time(fc.window_time);
    }

    inline
    void unpack_freq_cap(
      FreqCap& res, const AdServer::Commons::FreqCapInfo& fc)
    {
      res.fc_id = fc.fc_id;
      res.lifelimit = fc.lifelimit;
      res.period = CorbaAlgs::unpack_time(fc.period);
      res.window_limit = fc.window_limit;
      res.window_time = CorbaAlgs::unpack_time(fc.window_time);
    }
  }
}

#endif
