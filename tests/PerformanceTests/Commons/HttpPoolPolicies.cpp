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


#include "HttpPoolPolicies.hpp"

// class HttpPoolPolicy

HttpPoolPolicy::HttpPoolPolicy(
  Logging::Logger* logger,
  unsigned connections_per_server,
  unsigned connections_per_threads,
  const time_t  timeout) throw (eh::Exception) :
  PoolPolicySimpleDecider(connections_per_server, connections_per_threads),
  PoolPolicyWaitRequests(REQUEST_QUEUE_SIZE),
  PoolPolicySimpleTimeout(timeout),
  logger_(logger),
  request_queue_size_(0)
{
}

HttpPoolPolicy::~HttpPoolPolicy() throw ()
{
}

void HttpPoolPolicy::request_constructing() throw (eh::Exception)
{
  __gnu_cxx::__atomic_add(&request_queue_size_, 1);
  condition_.broadcast();
  PoolPolicyWaitRequests::request_constructing();
}

void HttpPoolPolicy::request_destroying() throw ()
{
  __gnu_cxx::__atomic_add(&request_queue_size_, -1);
  condition_.broadcast();
  PoolPolicyWaitRequests::request_destroying();
}

void HttpPoolPolicy::wait_empty() throw()
{
  while (request_queue_size_ > 0)
  {
    condition_.wait();
  }
}

unsigned long HttpPoolPolicy::get_request_queue_size()
{
  return request_queue_size_;
}

unsigned long HttpPoolPolicy::get_threads_count()
{
  return get_threads_().size();
}

unsigned long HttpPoolPolicy::get_connections_count()
{
  return get_connections_().size();
}
  
void HttpPoolPolicy::report_error(
  Generics::ActiveObjectCallback::Severity,
  const String::SubString& description,
  const char* ) throw()
{
  logger_->stream(Logging::Logger::WARNING) <<
    "HttpPoolPolicies, error: " << description;
}


