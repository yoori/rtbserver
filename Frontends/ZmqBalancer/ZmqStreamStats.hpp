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

#ifndef ZMQBALANCER_ZMQSTREAMSTATS_HPP_
#define ZMQBALANCER_ZMQSTREAMSTATS_HPP_

#include <ReferenceCounting/AtomicImpl.hpp>
#include <ReferenceCounting/SmartPtr.hpp>
#include <Generics/HashTableAdapters.hpp>
#include <Commons/AtomicInt.hpp>

namespace AdServer
{
  class ZmqStreamStats: public ReferenceCounting::AtomicImpl
  {
  public:
    ZmqStreamStats() throw();

    void
    add_dropped_messages(unsigned long dropped_count)
      throw();

    void
    add_sent_messages(
      unsigned long sent_count,
      unsigned long sent_size)
      throw();

    void
    add_received_messages(
      unsigned long received_count,
      unsigned long received_size)
      throw();

    unsigned long
    dropped_messages() const throw();

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
    static const Generics::StringHashAdapter DROPPED_MESSAGES_COUNT_;

  protected:
    Algs::AtomicUInt dropped_messages_count_;
    Algs::AtomicUInt sent_messages_count_;
    Algs::AtomicUInt sent_messages_size_;
    Algs::AtomicUInt received_messages_count_;
    Algs::AtomicUInt received_messages_size_;
  };

  typedef ReferenceCounting::SmartPtr<ZmqStreamStats>
    ZmqStreamStats_var;
}

namespace AdServer
{
  template <typename Functor>
  void
  ZmqStreamStats::enumerate_all(Functor& functor) const
    throw (eh::Exception)
  {
    functor(5);
    functor(DROPPED_MESSAGES_COUNT_, static_cast<int>(dropped_messages_count_));
    functor(SENT_MESSAGES_COUNT_, static_cast<int>(sent_messages_count_));
    functor(SENT_MESSAGES_SIZE_, static_cast<int>(sent_messages_size_));
    functor(RECEIVED_MESSAGES_COUNT_, static_cast<int>(received_messages_count_));
    functor(RECEIVED_MESSAGES_SIZE_, static_cast<int>(received_messages_size_));
  }
}

#endif
