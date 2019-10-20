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


#include "AdServerClient.hpp"
#include "BenchmarkStorage.hpp"
#include <tests/AutoTests/Commons/AdClient.hpp>

// class  AdServerClientBase

AdServerClientBase::AdServerClientBase(
  unsigned long client_id,
  HttpInterface* pool,
  BenchmarkStorage* storage,
  Logging::Logger* logger) :
  client_id_(client_id),
  client_(pool),
  storage_(storage),
  logger_(logger)
{
}

AdServerClientBase::~AdServerClientBase() throw()
{
}

unsigned long AdServerClientBase::id() const
{
  return client_id_;
}


void
AdServerClientBase::on_response(
  unsigned long client_id,
  const HTTP::ResponseInformation& data,
  bool /*is_opted_out*/,
  unsigned long /*ccid*/,
  const AdvertiserResponse* ad_response) throw ()
{

  std::string client_uid;

  uid(client_uid);

  if (logger_->log_level() >=  Logging::Logger::TRACE)
  {
    std::ostringstream out;
    out << "Client# " << client_id << " response for request '"
        << data.http_request() << std::endl << "  Headers:" << std::endl;
      HTTP::HeaderList::const_iterator
      h_it(data.response_headers().begin());
    for (; h_it != data.response_headers().end(); ++h_it)
    {
      out << "    " << h_it->name << "=" << h_it->value << std::endl;
    }
    out << "  Response Body:" << std::endl << data.body() << std::endl;
    logger_->log(out.str(), Logging::Logger::DEBUG);
  }

  storage_->stats()->add_response(
    client_uid.c_str(),
    ad_response);
  if (ad_response)
  {
    bool check_passback = true;
    if (!ad_response->click_url.empty())
    {
      storage_->push_request_pair(FrontendType::click,
                                  this,
                                  ad_response->click_url.c_str());
      check_passback = false;
    }
    if (!ad_response->action_adv_url.empty())
    {
      storage_->push_request_pair(FrontendType::action,
                                  this,
                                  ad_response->action_adv_url.c_str());
      check_passback = false;
    }
    if (check_passback && !ad_response->passback_url.empty())
    {
      storage_->push_request_pair(FrontendType::passback,
                                  this,
                                  ad_response->passback_url.c_str());

    }

  }
}

void
AdServerClientBase::on_error(
  const String::SubString& description,
  const HTTP::ResponseInformation& data,
  bool /*is_opted_out*/) throw ()
{
  if (logger_->log_level() >=  Logging::Logger::DEBUG)
  {
    Stream::Error err;
    err << "Response error (" <<
      data.http_request() << "):" <<
      description << std::endl;
    err << "  Response headers: " << std::endl;
    HTTP::HeaderList::const_iterator
      h_it(data.response_headers().begin());
    for (; h_it != data.response_headers().end(); ++h_it)
    {
      err << "    " << h_it->name << "=" << h_it->value << std::endl;
    }
    err << "  Response Body:" << std::endl << data.body() << std::endl;
    logger_->log(err.str(), Logging::Logger::DEBUG);
  }
  storage_->stats()->incr_errors();
}

void
AdServerClientBase::send_request(
  BaseRequest* request)
{

  // Prepare request
  ResponseCallback_var response_cb_(request);
  std::string url(request->url());
  HeaderList headers;
  request->headers(headers);

  // Log request
  if (logger_->log_level() >=  Logging::Logger::TRACE)
  {
    std::ostringstream ostr;
    ostr << request << std::endl;
    logger_->log(ostr.str(), Logging::Logger::TRACE);
  }

  // Send request
  if (request->isGet())
  {
    client_->add_get_request(url.c_str(),
                             response_cb_,
                             HTTP::HttpServer(),
                             headers);
  }
  else
  {
    // Prepare body
    std::string body_cont(request->body());
    client_->add_post_request(url.c_str(),
                              response_cb_,
                              body_cont,
                              HTTP::HttpServer(),
                              headers);
  }
}

// class OptOutClient

OptOutClient::OptOutClient(
  unsigned long client_id,
  HttpInterface* pool,
  BenchmarkStorage* storage,
  Logging::Logger* logger)
  : AdServerClientBase(client_id, pool, storage, logger)
{
}

OptOutClient::~OptOutClient() throw()
{
}

void OptOutClient::uid(std::string& uid_) const
{
  uid_ = "OptedOutClient";
}

// OpenRTBClient

OpenRTBClient::OpenRTBClient(
  unsigned long client_id,
  HttpInterface* pool,
  BenchmarkStorage* storage,
  Logging::Logger* logger) :
  AdServerClientBase(client_id, pool, storage, logger),
  uid_(AutoTest::generate_uid())
{ }

OpenRTBClient::~OpenRTBClient() throw()
{
}

void OpenRTBClient::uid(std::string& uid) const
{
  uid = uid_;
}


// class OptInClient

OptInClient::OptInClient(
  unsigned long client_id,
  HttpInterface* pool,
  BenchmarkStorage* storage,
  Logging::Logger* logger)
  : AdServerClientBase(client_id, pool, storage, logger),
    cookie_(new CookiePoolPtr(new UidCookies)),
    client_var_(CreateCookieClient(pool, cookie_.in()))
{
  client_ = client_var_.in();
}

OptInClient::~OptInClient() throw()
{
}

void
OptInClient::UidCookies::uid(
  std::string& uid_) const throw (eh::Exception)
{
  for (const_iterator cookie_it(begin()); cookie_it != end(); ++cookie_it)
  {
    if (cookie_it->name == "uid")
    {
      uid_ = cookie_it->value;
      return;
    }
  }
}

void OptInClient::uid(std::string& uid_) const
{
  cookie_->as<UidCookies>()->uid(uid_);
}
