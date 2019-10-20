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

/* $Id: ProfilingServerStats.hpp 184904 2018-02-28 10:22:48Z artem_nikitin $
* @file ProfilingServerStats.hpp
* @author Artem V. Nikitin (artem_nikitin@ocslab.com)
* Profiling server statistics
*/
#pragma once

#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/HashTableAdapters.hpp>
#include <Commons/AtomicInt.hpp>
#include <Frontends/ProfilingServer/DMPKafkaStreamer.hpp>

namespace AdServer
{
  namespace Profiling
  {
    class ProfilingServerStats : public ReferenceCounting::AtomicImpl
    {
    public:
      ProfilingServerStats(
        DMPKafkaStreamer* streamer) throw();
      
      void
      add_filtered_messages(unsigned long filtered_count)
        throw();

      void
      add_received_messages(
        unsigned long received_count,
        unsigned long received_size)
        throw();
      
      unsigned long
      filtered_messages() const throw();

      unsigned long
      kafka_errors() const throw();
      
      unsigned long
      sent_messages() const throw();

      unsigned long
      sent_messages_size() const throw();

      unsigned long
      received_messages() const throw();
      
      unsigned long
      received_messages_size() const throw();

      template <typename Functor>
      void
      enumerate_all(Functor& functor) const throw (eh::Exception);

    protected:
      static const Generics::StringHashAdapter SENT_MESSAGES_COUNT_;
      static const Generics::StringHashAdapter SENT_MESSAGES_SIZE_;
      static const Generics::StringHashAdapter RECEIVED_MESSAGES_COUNT_;
      static const Generics::StringHashAdapter RECEIVED_MESSAGES_SIZE_;
      static const Generics::StringHashAdapter FILTERED_MESSAGES_COUNT_;
      static const Generics::StringHashAdapter KAFKA_ERRORS_COUNT_;

      ~ProfilingServerStats() throw() = default;
        
    protected:
      DMPKafkaStreamer_var streamer_;
      Algs::AtomicUInt filtered_messages_count_;
      Algs::AtomicUInt received_messages_count_;
      Algs::AtomicUInt received_messages_size_;
    };

    typedef ReferenceCounting::SmartPtr<ProfilingServerStats> ProfilingServerStats_var;
  }
}

#include "ProfilingServerStats.ipp"
