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


#include "Benchmark.hpp"

// class BenchmarkBase

BenchmarkBase::BenchmarkBase(
  const BenchmarkConfig* config,
  BenchmarkStorage* storage,
  HttpInterface* pool,
  Logging::Logger* logger) :
  config_(config),
  storage_(storage),
  pool_(pool),
  logger_(logger)
{
}

BenchmarkBase::~BenchmarkBase() throw()
{
}

bool
BenchmarkBase::run()
{
  if (config_->initial)
  {
      storage_->clear();
      for (unsigned long id=0; id < config_->size; ++id)
      {
        AdServerClientBase* client = make_client(id);
        storage_->push_client(client);
        send_request(client);
      }
  }
  else
  {
    if (!storage_->client_size())
    {
      logger_->stream(Logging::Logger::CRITICAL) <<
        "Error: There aren't stored "
        "clients for benchmark!";
      return false;
    }
    for (unsigned long id=0; id < config_->size; ++id)
    {
      send_storage_request();
    }
  }
  return true;
}

AdServerClientBase*
BenchmarkBase::make_client(unsigned long client_id)
{
  switch (config_->client_type)
  {
  case ClientType::optout:
    return new OptOutClient(client_id, pool_, storage_, logger_);

  case ClientType::openrtb:
    return new OpenRTBClient(client_id, pool_, storage_, logger_);

  default:
    return  new OptInClient(client_id, pool_, storage_, logger_);
  }
}


BaseRequest*
BenchmarkBase::make_request(
  AdServerClientBase* client,
  const char* url)
{
  std::string uid;
  client->uid(uid);
  switch (config_->frontend_type)
  {
  case FrontendType::click:
    return new ClickRequest(
      client,
      client->id(),
      config_->client_type == ClientType::optout,
      config_->config,
      url);

  case FrontendType::passback:
    return new PassbackRequest(
      client,
      client->id(),
      config_->client_type == ClientType::optout,
      config_->config,
      url);

  case FrontendType::action:
    return new ActionRequest(
      client,
      client->id(),
      config_->client_type == ClientType::optout,
      config_->config,
      url);

  case  FrontendType::nslookup:
    return new NSLookupRequest(
      client,
      client->id(),
      config_->client_type == ClientType::optout,
      config_->config,
      config_->server.c_str(),
      false);

  case  FrontendType::userbind:
    return new UserBindRequest(
      client,
      client->id(),
      uid,
      config_->config,
      config_->server.c_str());

  case  FrontendType::openrtb:
    return new OpenRTBRequest(
      client,
      client->id(),
      uid,
      config_->config,
      config_->server.c_str());

  default:
      return 0;

  }
  return 0;
}


void
BenchmarkBase::send_request(AdServerClientBase* )
{
  throw NotSupported("Not supported for this request type");
}


// class BenchmarkBase

Benchmark::Benchmark(
  const BenchmarkConfig* config,
  BenchmarkStorage* storage,
  HttpInterface* pool,
  Logging::Logger* logger) :
  BenchmarkBase(config, storage, pool, logger)
{
}

Benchmark::~Benchmark() throw()
{
}

void Benchmark::send_request(AdServerClientBase* client)
{
  client->send_request(make_request(client));
}

void Benchmark::send_storage_request()
{
  AdServerClientBase* client = storage_->get_client();
  send_request(client);
}

// class GeneratedBenchmark

GeneratedBenchmark::GeneratedBenchmark(
  const BenchmarkConfig* config,
  BenchmarkStorage* storage,
  HttpInterface* pool,
  Logging::Logger* logger) :
  BenchmarkBase(config, storage, pool, logger)
{
}

GeneratedBenchmark::~GeneratedBenchmark() throw()
{
}

bool
GeneratedBenchmark::run()
{
  if (!storage_->request_size(config_->frontend_type))
  {
    logger_->stream(Logging::Logger::CRITICAL) <<
      "Error: There aren't stored "
      "requests for benchmark!";
    return false;
  }
  return BenchmarkBase::run();
}

void GeneratedBenchmark::send_storage_request()
{
  std::string url;
  AdServerClientBase* client =
    storage_->get_request_pair(config_->frontend_type, url);
  client->send_request(make_request(client, url.c_str()));
}
