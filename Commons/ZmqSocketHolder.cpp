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

#include <Stream/MemoryStream.hpp>
#include "ZmqSocketHolder.hpp"

namespace AdServer
{
namespace Commons
{
  // ZmqSocketHolder::SendGuard
  ZmqSocketHolder::SendGuard::SendGuard(ZmqSocketHolder& holder)
    : socket_holder_(holder)
  {
    socket_holder_.lock_.lock();
  }

  ZmqSocketHolder::SendGuard::~SendGuard() throw()
  {
    socket_holder_.lock_.unlock();
  }

  bool
  ZmqSocketHolder::SendGuard::send(
    zmq::message_t& msg,
    int flags,
    const char* part_name)
    throw(eh::Exception)
  {
    bool send_res;

    try
    {
      send_res = socket_holder_.sock_.send(msg, flags);
    }
    catch (const eh::Exception& ex)
    {
      Stream::Error ostr;
      ostr << "On sending '" << part_name << "' "
        "caught zmq::exception: " << ex.what();
      throw Exception(ostr);
    }

    if(!send_res)
    {
      if(errno != EAGAIN)
      {
        Stream::Error ostr;
        ostr << "Can't send '" << part_name << "' part, errno: " <<
          strerror(errno);
        throw Exception(ostr);
      }

      return false;
    }

    return true;
  }

  // ZmqSocketHolder
  ZmqSocketHolder::ZmqSocketHolder(
    zmq::context_t& zmq_context,
    const char* address)
    throw(eh::Exception)
    : sock_(zmq_context, ZMQ_PUSH)
  {
    init_sock_();
    sock_.connect(address);
  }

  ZmqSocketHolder::ZmqSocketHolder(
    zmq::context_t& zmq_context,
    int type)
    throw(eh::Exception)
    : sock_(zmq_context, type)
  {
    init_sock_();
  }

  void
  ZmqSocketHolder::init_sock_()
  {
#if ZMQ_VERSION >= ZMQ_MAKE_VERSION(3, 0, 0)
    const int hwm = 1000;
    sock_.setsockopt(ZMQ_RCVHWM, &hwm, sizeof(hwm));
    sock_.setsockopt(ZMQ_SNDHWM, &hwm, sizeof(hwm));
#else
    const uint64_t hwm = 1000;
    sock_.setsockopt(ZMQ_HWM, &hwm, sizeof(hwm));
#endif

    const int recv_timeout = 500; // 0.5 sec
    sock_.setsockopt(ZMQ_RCVTIMEO, &recv_timeout, sizeof(recv_timeout));

    const int linger = 10; // 10 ms
    sock_.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    const int reconnect_interval = 100; // 100 ms
    sock_.setsockopt(ZMQ_RECONNECT_IVL,
      &reconnect_interval, sizeof(reconnect_interval));
  }
}
}
