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


#ifndef _ADSERVERCLIENTS_HPP
#define _ADSERVERCLIENTS_HPP

#include <HTTP/HttpClient.hpp>
#include <tests/PerformanceTests/Commons/QuerySenderBase.hpp>
#include <tests/PerformanceTests/Commons/Request.hpp>
#include <Logger/Logger.hpp>
#include "Statistics.hpp"

class BenchmarkStorage;

/**
 * @class AdServerClientBase
 * @brief Base server client for benchmark tests
 */
class AdServerClientBase :  public QuerySenderBase,
                            public ReferenceCounting::AtomicImpl
{
public:
  /**
   * @brief Constructor
   *
   * @param client local (test) ID
   * @param HTTP pool
   * @param benchmark storage
   * @param logger
   */
  AdServerClientBase(
    unsigned long client_id,
    HttpInterface* pool,
    BenchmarkStorage* storage,
    Logging::Logger* logger);

  /**
   * @brief Destructor
   */
  virtual ~AdServerClientBase() throw();

  /**
   * @brief Get client ID
   *
   * @return client ID
   */
  unsigned long id() const;

  /**
   * @brief Get client UUID
   *
   * @param [out] client global (server) UUID
   */
  virtual void uid(std::string& uid_) const  = 0;

  /**
   * @brief Send request (add to HTTP pool)
   *
   * @param request
   */
  virtual void send_request(BaseRequest* request);

  /**
   * @brief Callback calling from BaseRequest after getting correct response.
   *
   * @param Client ID
   * @param HTTP request URL
   * @param response code
   * @param is opted out request
   * @param cc_id
   * @param advertising response (baased on Debug-Info header)
   */
  virtual void on_response(
    unsigned long client_id,
    const HTTP::ResponseInformation& data,
    bool is_opted_out,
    unsigned long ccid = 0,
    const AdvertiserResponse*
    ad_response = 0) throw ();

  /**
   * @brief Callback calling from BaseRequest after getting HTTP error.
   *
   * @param error description
   * @param HTTP request URL
   * @param is opted out request
   */
  virtual void on_error(
    const String::SubString& description,
    const HTTP::ResponseInformation& data,
    bool is_opted_out) throw ();


protected:
  unsigned long client_id_;    // client ID
  HttpInterface* client_;      // HTTP client
  BenchmarkStorage* storage_;  // benchmark storage
  Logging::Logger* logger_;             // logger
};


/**
 * @class OptOutClient
 * @brief Opted out client (with no cookies)
 */
class OptOutClient : public AdServerClientBase
{
public:

  /**
   * @brief Constructor
   *
   * @param client local (test) ID
   * @param HTTP pool
   * @param benchmark storage
   * @param logger
   */
  OptOutClient(
    unsigned long client_id,
    HttpInterface* pool,
    BenchmarkStorage* storage,
    Logging::Logger* logger);

  /**
   * @brief Destructor
   */
  virtual ~OptOutClient() throw();

  /**
   * @brief Get client UUID
   *
   * @param [out] client global (server) UUID
   */
  virtual void uid(std::string& uid_) const;

};

/**
 * @class OpenRTBClient
 * @brief OpenRTB client (identify by ssp_user_id)
 */
class OpenRTBClient: public AdServerClientBase
{
public:

  /**
   * @brief Constructor
   *
   * @param client local (test) ID
   * @param HTTP pool
   * @param benchmark storage
   * @param logger
   */
  OpenRTBClient(
    unsigned long client_id,
    HttpInterface* pool,
    BenchmarkStorage* storage,
    Logging::Logger* logger);

  /**
   * @brief Destructor
   */
  virtual ~OpenRTBClient() throw();

  /**
   * @brief Get client UUID
   *
   * @param [out] client global (server) UUID
   */
  virtual void uid(std::string& uid) const;

private:
  std::string uid_;
};

/**
 * @class OptInClient
 * @brief Opted in client (with no cookies)
 */
class OptInClient : public AdServerClientBase
{
public:
  /**
   * @brief Constructor
   *
   * @param client local (test) ID
   * @param HTTP pool
   * @param benchmark storage
   * @param logger
   */
  OptInClient(
    unsigned long client_id,
    HttpInterface* pool,
    BenchmarkStorage* storage,
    Logging::Logger* logger);

  /**
   * @brief Destructor
   */
  virtual ~OptInClient() throw();

  /**
   * @brief Get client UUID
   *
   * @param [out] client global (server) UUID
   */
  virtual void uid(std::string& uid_) const;

private:
  class UidCookies : public HTTP::ClientCookieFacility
  {
  public:
    void
    uid(std::string& uid_) const throw (eh::Exception);
  };

  CookiePool_var cookie_;        // cookie container
  HttpInterface_var client_var_; // client
};

typedef ReferenceCounting::SmartPtr<AdServerClientBase> AdServerClient_var;


#endif  //_ADSERVERCLIENTS_HPP
