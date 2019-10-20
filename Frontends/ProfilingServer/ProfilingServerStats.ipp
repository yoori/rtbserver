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

/* $Id$
* @file ProfilingServerStats.ipp
* @author Artem V. Nikitin (artem_nikitin@ocslab.com)
* Profiling server statistics
*/

namespace AdServer
{
  namespace Profiling
  {
    template <typename Functor>
    void
    ProfilingServerStats::enumerate_all(Functor& functor) const
      throw (eh::Exception)
    {
      functor(6);
      functor(FILTERED_MESSAGES_COUNT_, static_cast<int>(filtered_messages_count_));
      functor(KAFKA_ERRORS_COUNT_, static_cast<int>(kafka_errors()));
      functor(SENT_MESSAGES_COUNT_, static_cast<int>(sent_messages()));
      functor(SENT_MESSAGES_SIZE_, static_cast<int>(sent_messages_size()));
      functor(RECEIVED_MESSAGES_COUNT_, static_cast<int>(received_messages_count_));
      functor(RECEIVED_MESSAGES_SIZE_, static_cast<int>(received_messages_size_));
    }
  }
}
