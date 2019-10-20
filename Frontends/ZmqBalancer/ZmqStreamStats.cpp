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

#include "ZmqStreamStats.hpp"

namespace AdServer
{
  const Generics::StringHashAdapter
  ZmqStreamStats::SENT_MESSAGES_COUNT_("sentMessagesCount");

  const Generics::StringHashAdapter
  ZmqStreamStats::SENT_MESSAGES_SIZE_("sentMessagesSize");

  const Generics::StringHashAdapter
  ZmqStreamStats::DROPPED_MESSAGES_COUNT_("droppedMessagesCount");

  const Generics::StringHashAdapter
  ZmqStreamStats::RECEIVED_MESSAGES_COUNT_("recvMessagesCount");

  const Generics::StringHashAdapter
  ZmqStreamStats::RECEIVED_MESSAGES_SIZE_("recvMessagesSize");


  ZmqStreamStats::ZmqStreamStats() throw()
    : dropped_messages_count_(0),
      sent_messages_count_(0),
      sent_messages_size_(0),
      received_messages_count_(0),
      received_messages_size_(0)
  {}

  void
  ZmqStreamStats::add_dropped_messages(unsigned long dropped_count)
    throw()
  {
    dropped_messages_count_ += dropped_count;
  }

  void
  ZmqStreamStats::add_sent_messages(
    unsigned long sent_count,
    unsigned long sent_size)
    throw()
  {
    sent_messages_count_ += sent_count;
    sent_messages_size_ += sent_size;
  }

  void
  ZmqStreamStats::add_received_messages(
    unsigned long received_count,
    unsigned long received_size)
    throw()
  {
    received_messages_count_ += received_count;
    received_messages_size_ += received_size;
  }

  unsigned long
  ZmqStreamStats::dropped_messages() const throw()
  {
    return dropped_messages_count_;
  }

  unsigned long
  ZmqStreamStats::sent_messages() const throw()
  {
    return sent_messages_count_;
  }

  unsigned long
  ZmqStreamStats::sent_messages_size() const throw()
  {
    return sent_messages_size_;
  }

  unsigned long
  ZmqStreamStats::received_messages() const throw()
  {
    return received_messages_count_;
  }

  unsigned long
  ZmqStreamStats::received_messages_size() const throw()
  {
    return received_messages_size_;
  }
}
